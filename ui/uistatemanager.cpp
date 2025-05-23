/*
  uistatemanager.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "uistatemanager.h"
#include "deferredtreeview.h"
#include "deferredtreeview_p.h"
#include "common/endpoint.h"
#include "common/settempvalue.h"

#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QSplitter>
#include <QSettings>
#include <QEvent>
#include <QScreen>
#include <QTimer>

#include <vector>

#define WIDGET_CUSTOMIZED "customized"

using namespace GammaRay;

namespace {
QAbstractItemView *headerView(QHeaderView *header)
{
    QWidget *view = header->parentWidget();

    while (!qobject_cast<QAbstractItemView *>(view))
        view = view->parentWidget();

    return qobject_cast<QAbstractItemView *>(view);
}

void distributeSpace(QList<int> &sizes, int size, int handleSize)
{
    std::vector<int> indexes;
    int usedSpace = 0;

    for (int i = 0; i < sizes.size(); ++i) {
        if (sizes[i] == -1)
            indexes.push_back(i);
        else
            usedSpace += sizes[i];
    }

    if (!indexes.empty()) {
        const int freeSpace = size - usedSpace - (sizes.size() * handleSize) - handleSize;
        const int space = freeSpace / indexes.size();
        for (auto i : indexes)
            sizes[i] = space;
    }
}

using QSplitterList = QList<QSplitter *>;
using QHeaderViewList = QList<QHeaderView *>;
}

UIStateManager::UIStateManager(QWidget *widget)
    : QObject(widget)
    , m_widget(widget)
    , m_stateSettings(new QSettings(this))
    , m_initialized(false)
    , m_settingsAccess(false)
    , m_resizing(false)
    , m_targetStateSource(nullptr)
    , m_targetRestoreMethodId(-1)
    , m_targetSaveMethodId(-1)
{
    Q_ASSERT(m_widget);

    m_widget->installEventFilter(this);
}

UIStateManager::~UIStateManager() = default;

QWidget *UIStateManager::widget() const
{
    return m_widget;
}

bool UIStateManager::initialized() const
{
    return m_initialized;
}

QList<QSplitter *> UIStateManager::splitters() const
{
    return m_widget->findChildren<QSplitter *>();
}

QList<QHeaderView *> UIStateManager::headers() const
{
    return m_widget->findChildren<QHeaderView *>();
}

void UIStateManager::setup()
{
    if (!Endpoint::instance()->isConnected()) // in-process mode, don't interfere with QSettings there
        return;
    Q_ASSERT(!m_initialized);
    if (!checkWidget(m_widget))
        return;

    m_initialized = true;
    m_stateSettings->beginGroup(QString::fromLatin1("UiState/%1").arg(widgetName(m_widget)));

    // Make sure objects names are unique.
    // Also make the comparison lower case as some QSettings implementations
    // are case sensitive.
    QSet<QString> knownNames;

    knownNames << widgetPath(m_widget);

    foreach (QSplitter *splitter, splitters()) {
        if (!checkWidget(splitter))
            continue;

        const QString name = widgetPath(splitter);

        if (knownNames.contains(name)) {
            qWarning() << Q_FUNC_INFO << "Duplicate widget name" << name << "in widget" << m_widget;
            continue;
        }

        knownNames << name;

        connect(splitter, &QSplitter::splitterMoved, this, &UIStateManager::widgetCustomized, Qt::UniqueConnection);
    }

    foreach (QHeaderView *header, headers()) {
        if (!checkWidget(header))
            continue;

        const QString name = widgetPath(header);

        if (knownNames.contains(name)) {
            qWarning() << Q_FUNC_INFO << "Duplicate widget name" << name << "in widget" << m_widget;
            continue;
        }

        knownNames << name;

        connect(header, &QHeaderView::sectionResized, this, &UIStateManager::widgetCustomized, Qt::UniqueConnection);
        connect(header, &QHeaderView::sectionCountChanged, this,
                &UIStateManager::headerSectionCountChanged, Qt::UniqueConnection);

        QAbstractItemView *view = headerView(header);
        view->removeEventFilter(this);
        view->installEventFilter(this);
    }

    // Try to find methods on widget
    m_targetStateSource = m_widget->metaObject();
    m_targetRestoreMethodId = m_targetStateSource->indexOfMethod("restoreTargetState(QSettings*)");
    m_targetSaveMethodId = m_targetStateSource->indexOfMethod("saveTargetState(QSettings*)");

    // Here for inherited UIStateManager where widget is not modifiable (ie: QtC)
    if (m_targetRestoreMethodId == -1 || m_targetSaveMethodId == -1) {
        m_targetStateSource = metaObject();
        m_targetRestoreMethodId = m_targetStateSource->indexOfMethod("restoreTargetState(QSettings*)");
        m_targetSaveMethodId = m_targetStateSource->indexOfMethod("saveTargetState(QSettings*)");
    }

    if (m_targetRestoreMethodId == -1 || m_targetSaveMethodId == -1) {
        m_targetStateSource = nullptr;
        m_targetRestoreMethodId = -1;
        m_targetSaveMethodId = -1;
    }

    restoreState();
}

void UIStateManager::restoreState()
{
    if (!Endpoint::instance()->isConnected())
        return;

    if (!m_initialized) {
        qWarning() << Q_FUNC_INFO << "Attempting to restoreState for a not yet initialized state manager.";
        return;
    }

    if (m_settingsAccess) {
        qWarning() << Q_FUNC_INFO << "Recursive restore/save state detected" << widgetPath(m_widget) << m_widget;
        Q_ASSERT(false);
        return;
    }

    m_settingsAccess = true;

    restoreWindowState();
    restoreSplitterState();
    restoreHeaderState();

    // Allow restore state per end point
    if (m_targetStateSource) {
        Q_ASSERT(!Endpoint::instance()->key().isEmpty());
        m_stateSettings->beginGroup(Endpoint::instance()->key());
        QMetaMethod method = m_targetStateSource->method(m_targetRestoreMethodId);
        QObject *target = m_targetStateSource == m_widget->metaObject() ? qobject_cast<QObject *>(m_widget) : this;
        // clang-format off
        method.invoke(target, Q_ARG(QSettings*, m_stateSettings));
        // clang-format on
        m_stateSettings->endGroup();
    }

    m_settingsAccess = false;
}

void UIStateManager::saveState()
{
    if (!Endpoint::instance()->isConnected())
        return;

    if (!m_initialized) {
        qWarning() << Q_FUNC_INFO << "Attempting to saveState for a not yet initialized state manager.";
        return;
    }

    if (m_settingsAccess) {
        qWarning() << Q_FUNC_INFO << "Recursive save/restore state detected" << widgetPath(m_widget) << m_widget;
        return;
    }

    m_settingsAccess = true;

    // Allow save state per end point
    if (m_targetStateSource) {
        Q_ASSERT(!Endpoint::instance()->key().isEmpty());
        m_stateSettings->beginGroup(Endpoint::instance()->key());
        QMetaMethod method = m_targetStateSource->method(m_targetSaveMethodId);
        QObject *target = m_targetStateSource == m_widget->metaObject() ? qobject_cast<QObject *>(m_widget) : this;
        // clang-format off
        method.invoke(target, Q_ARG(QSettings*, m_stateSettings));
        // clang-format on
        m_stateSettings->endGroup();
    }

    saveWindowState();
    saveSplitterState();
    saveHeaderState();

    m_settingsAccess = false;
}

bool UIStateManager::eventFilter(QObject *object, QEvent *event)
{
    const bool connected = Endpoint::instance()->isConnected();

    if (connected && object == m_widget) {
        if (event->type() == QEvent::Hide) {
            if (m_initialized)
                saveState();
        }
    }

    const bool result = QObject::eventFilter(object, event);

    if (connected && object == m_widget) {
        if (event->type() == QEvent::Show) {
            if (!m_initialized)
                setup();
        }
    }

    if (connected && event->type() == QEvent::Resize) {
        if (m_initialized && !m_resizing)
            widgetResized(qobject_cast<QWidget *>(object));
    }

    return result;
}

QString UIStateManager::widgetName(QWidget *widget)
{
    return (widget->objectName().isEmpty()
                ? QString::fromLatin1(widget->metaObject()->className())
                : widget->objectName())
        .toLower();
}

QString UIStateManager::widgetPath(QWidget *widget) const
{
    QStringList path(widgetName(widget));
    if (widget != m_widget) {
        QWidget *parent = widget->parentWidget();
        while (parent && parent != m_widget) {
            path.prepend(widgetName(parent));
            parent = parent->parentWidget();
        }
    }
    return path.join(QLatin1String("-"));
}

UISizeVector UIStateManager::defaultSizes(QSplitter *splitter) const
{
    return checkWidget(splitter) ? m_defaultSplitterSizes.value(widgetPath(splitter)) : UISizeVector();
}

void UIStateManager::setDefaultSizes(QSplitter *splitter, const UISizeVector &defaultSizes)
{
    if (checkWidget(splitter))
        m_defaultSplitterSizes[widgetPath(splitter)] = defaultSizes;
}

UISizeVector UIStateManager::defaultSizes(QHeaderView *header) const
{
    return checkWidget(header) ? m_defaultHeaderSizes.value(widgetPath(header)) : UISizeVector();
}

void UIStateManager::setDefaultSizes(QHeaderView *header, const UISizeVector &defaultSizes)
{
    if (checkWidget(header))
        m_defaultHeaderSizes[widgetPath(header)] = defaultSizes;
}

void UIStateManager::reset()
{
    if (m_initialized) {
        m_initialized = false;
        m_stateSettings->endGroup();
    }
    setup();
}

QString UIStateManager::widgetGeometryKey(QWidget *widget) const
{
    return QString::fromLatin1("%1Geometry").arg(widgetPath(widget));
}

QString UIStateManager::widgetStateKey(QWidget *widget) const
{
    return QString::fromLatin1("%1State").arg(widgetPath(widget));
}

QString UIStateManager::widgetStateSectionsKey(QWidget *widget) const
{
    return QString::fromLatin1("%1StateSections").arg(widgetPath(widget));
}

bool UIStateManager::checkWidget(QWidget *widget) const
{
    if (widget->objectName().isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Widget with no name" << widget << widgetPath(widget)
                   << "in widget" << m_widget;
        return false;
    }

    return true;
}

int UIStateManager::percentToInt(const QString &size)
{
    return size.left(size.length() - 1).toInt(); // clazy:exclude=qstring-ref due to Qt4 support
}

void UIStateManager::restoreWindowState()
{
    QMainWindow *window = qobject_cast<QMainWindow *>(m_widget);

    if (window) {
        const QByteArray geometry = m_stateSettings->value(widgetGeometryKey(m_widget)).toByteArray();
        const QByteArray state = m_stateSettings->value(widgetStateKey(m_widget)).toByteArray();

        if (geometry.isEmpty()) {
            const QRect area = m_widget->screen()->availableGeometry();
            QRect rect(QPoint(), QSize(1024, 768));
            rect.moveCenter(area.center());
            m_widget->setGeometry(rect);
        } else {
            if (!m_resizing) {
                window->restoreGeometry(geometry);
                window->restoreState(state);
            }
        }
    }
}

void UIStateManager::saveWindowState()
{
    QMainWindow *window = qobject_cast<QMainWindow *>(m_widget);

    if (window) {
        m_stateSettings->setValue(widgetGeometryKey(m_widget), window->saveGeometry());
        m_stateSettings->setValue(widgetStateKey(m_widget), window->saveState());
    }
}

void UIStateManager::restoreSplitterState(QSplitter *splitter)
{
    const QSplitterList splitters = splitter ? (QSplitterList() << splitter) : this->splitters();
    for (QSplitter *splitter : splitters) {
        if (!checkWidget(splitter))
            continue;

        const QByteArray state = m_stateSettings->value(widgetStateKey(splitter)).toByteArray();

        if (state.isEmpty()) {
            const UISizeVector defaultSizes = this->defaultSizes(splitter);

            if (!defaultSizes.isEmpty()) {
                Q_ASSERT(defaultSizes.size() == splitter->count());

                QList<int> sizes;
                sizes.reserve(defaultSizes.size());

                for (const QVariant &size : defaultSizes) {
                    switch (size.typeId()) {
                    case QMetaType::Int: // Pixels
                        sizes << size.toInt();
                        break;

                    case QMetaType::QString: { // Percent
                        int value = percentToInt(size.toString());
                        if (value == -1)
                            sizes << value;
                        else if (splitter->orientation() == Qt::Horizontal)
                            sizes << splitter->width() * value / 100;
                        else
                            sizes << splitter->height() * value / 100;
                        break;
                    }

                    default:
                        Q_ASSERT(false);
                    }
                }

                distributeSpace(sizes,
                                splitter->orientation() == Qt::Horizontal ? splitter->width() : splitter->height(),
                                splitter->handleWidth());
                splitter->setSizes(sizes);
            }
        } else {
            if (!m_resizing) {
                splitter->restoreState(state);
                splitter->setProperty(WIDGET_CUSTOMIZED, true);
            }
        }
    }
}

void UIStateManager::saveSplitterState(QSplitter *splitter)
{
    const QSplitterList splitters = splitter ? (QSplitterList() << splitter) : this->splitters();
    for (QSplitter *splitter : splitters) {
        if (!checkWidget(splitter) || !splitter->property(WIDGET_CUSTOMIZED).toBool())
            continue;

        m_stateSettings->setValue(widgetStateKey(splitter), splitter->saveState());
    }
}

void UIStateManager::restoreHeaderState(QHeaderView *header)
{
    const QHeaderViewList headers = header ? (QHeaderViewList() << header) : this->headers();
    for (QHeaderView *header : headers) {
        if (header->orientation() == Qt::Vertical || !checkWidget(header) || header->count() == 0)
            continue;

        const QByteArray state = m_stateSettings->value(widgetStateKey(header)).toByteArray();

        if (state.isEmpty()) {
            const UISizeVector defaultSizes = this->defaultSizes(header);
            QAbstractItemView *view = headerView(header);

            if (!defaultSizes.isEmpty()) {
                Q_ASSERT(defaultSizes.size() == header->count());

                QList<int> sizes;
                sizes.reserve(defaultSizes.size());

                int i = 0;
                for (auto it = defaultSizes.constBegin(), end = defaultSizes.constEnd(); it != end;
                     ++it) {
                    int size = 0;

                    switch ((*it).typeId()) {
                    case QMetaType::Int: // Pixels
                        size = (*it).toInt();
                        break;

                    case QMetaType::QString: // Percent
                        size = percentToInt((*it).toString());
                        if (size == -1) {
                        } else if (header->orientation() == Qt::Horizontal) {
                            size = view->width() * size / 100;
                        } else {
                            size = view->height() * size / 100;
                        }
                        break;

                    default:
                        Q_ASSERT(false);
                    }

                    switch (header->sectionResizeMode(i)) {
                    case QHeaderView::Interactive:
                    case QHeaderView::Fixed:
                        header->resizeSection(i, size == -1 ? header->sectionSizeHint(i) : size);
                        break;

                    default:
                        break;
                    }

                    i++;
                }
            }
        } else {
            if (!m_resizing) {
                const int count = m_stateSettings->value(widgetStateSectionsKey(header), -1).toInt();
                if (count == header->count()) {
                    header->restoreState(state);
                    header->setProperty(WIDGET_CUSTOMIZED, true);
                } else {
                    // QHeaderView is not able to restore state correctly when the column count changed
                    // and lead to crash... let clear the settings.
                    m_stateSettings->remove(widgetStateSectionsKey(header));
                    m_stateSettings->remove(widgetStateKey(header));
                }
            }
        }
    }
}

void UIStateManager::saveHeaderState(QHeaderView *header)
{
    const QHeaderViewList headers = header ? (QHeaderViewList() << header) : this->headers();
    for (QHeaderView *header : headers) {
        if (header->orientation() == Qt::Vertical || !checkWidget(header) || header->count() == 0
            || !header->property(WIDGET_CUSTOMIZED).toBool())
            continue;

        m_stateSettings->setValue(widgetStateSectionsKey(header), header->count());
        m_stateSettings->setValue(widgetStateKey(header), header->saveState());
    }
}

void UIStateManager::headerSectionCountChanged()
{
    auto headerView = qobject_cast<QHeaderView *>(sender());
    // Delay the call to restoreHeaderState to avoid multiple changes in flight at the QAIM level
    // E.g. we might be here because of columnsInserted() (which just finished, but not all receivers were told yet)
    // and restoring will sort() the model, which will emit layoutChanged(). Separate the two so QAbstractItemModelTester doesn't abort.
    QMetaObject::invokeMethod(
        this, [this, headerView] {
            restoreHeaderState(headerView);
        },
        Qt::QueuedConnection);
}

void UIStateManager::widgetResized(QWidget *widget)
{
    Util::SetTempValue<bool> guard(m_resizing, true);

    if (widget == m_widget) {
        restoreSplitterState();
        restoreHeaderState();
    } else {
        foreach (QHeaderView *header, widget->findChildren<QHeaderView *>())
            restoreHeaderState(header);
    }
}

void UIStateManager::widgetCustomized()
{
    QSplitter *splitter = qobject_cast<QSplitter *>(sender());
    HeaderView *header = qobject_cast<HeaderView *>(sender());

    if (splitter) {
        splitter->setProperty(WIDGET_CUSTOMIZED, true);
        saveSplitterState(splitter);
    } else if (header) {
        // There is no way to know if a resize is manual or programamtically...
        // So only cast against our HeaderView which has a isState() member.
        if (header->isState(HeaderView::ResizeSection)) {
            header->setProperty(WIDGET_CUSTOMIZED, true);
            saveHeaderState(header);
        }
    }
}
