/*
  widgetinspectorserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config-gammaray.h>

#include "widgetinspectorserver.h"
#include "widgetpaintanalyzerextension.h"
#include "waextension/widgetattributeextension.h"

#include "overlaywidget.h"
#include "widgettreemodel.h"
#include "widget3dmodel.h"

#include "core/propertycontroller.h"
#include "core/metaobject.h"
#include "core/metaobjectrepository.h"
#include "core/varianthandler.h"
#include "core/probesettings.h"
#include "core/objecttypefilterproxymodel.h"
#include "core/probeinterface.h"
#include "core/probeguard.h"
#include <core/paintanalyzer.h>
#include <core/remoteviewserver.h>
#include <core/remote/serverproxymodel.h>

#include "common/objectbroker.h"
#include "common/settempvalue.h"
#include "common/metatypedeclarations.h"
#include "common/modelutils.h"
#include "common/objectmodel.h"
#include "common/paths.h"
#include <common/probecontrollerinterface.h>
#include <common/remoteviewframe.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QAction>
#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDialog>
#include <QItemSelectionModel>
#include <QLayout>
#include <QLibrary>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QMainWindow>
#include <QMouseEvent>
#include <QEvent>
#include <QStyle>
#include <QToolButton>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWindow>
#endif

#include <iostream>

Q_DECLARE_METATYPE(const QStyle *)
Q_DECLARE_METATYPE(QSizePolicy::ControlType)
Q_DECLARE_METATYPE(QSizePolicy::ControlTypes)
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::Orientations)
Q_DECLARE_METATYPE(Qt::Alignment)
Q_DECLARE_METATYPE(QSizePolicy::Policy)
#endif

using namespace GammaRay;
using namespace std;

static bool isGoodCandidateWidget(QWidget *widget)
{
    if (!widget->isVisible() || widget->testAttribute(Qt::WA_NoSystemBackground) ||
            widget->metaObject() == &QWidget::staticMetaObject) {
        return false;
    }

    return true;
}

WidgetInspectorServer::WidgetInspectorServer(ProbeInterface *probe, QObject *parent)
    : WidgetInspectorInterface(parent)
    , m_externalExportActions(new QLibrary(this))
    , m_propertyController(new PropertyController(objectName(), this))
    , m_paintAnalyzer(new PaintAnalyzer(QStringLiteral("com.kdab.GammaRay.WidgetPaintAnalyzer"),
                                        this))
    , m_remoteView(new RemoteViewServer(QStringLiteral("com.kdab.GammaRay.WidgetRemoteView"), this))
    , m_probe(probe)
{
    registerWidgetMetaTypes();
    registerVariantHandlers();
    probe->installGlobalEventFilter(this);
    PropertyController::registerExtension<WidgetPaintAnalyzerExtension>();
    PropertyController::registerExtension<WidgetAttributeExtension>();

    connect(m_remoteView, SIGNAL(requestUpdate()), this, SLOT(updateWidgetPreview()));

    recreateOverlayWidget();

    WidgetTreeModel *widgetFilterProxy = new WidgetTreeModel(this);
    widgetFilterProxy->setSourceModel(probe->objectTreeModel());

    auto widgetSearchProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    widgetSearchProxy->setSourceModel(widgetFilterProxy);
    widgetSearchProxy->addRole(ObjectModel::ObjectIdRole);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.WidgetTree"), widgetSearchProxy);

    auto widget3dmodel = new Widget3DModel(this);
    widget3dmodel->setSourceModel(m_probe->objectTreeModel());
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.Widget3DModel"), widget3dmodel);

    m_widgetSelectionModel = ObjectBroker::selectionModel(widgetSearchProxy);
    connect(m_widgetSelectionModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(widgetSelected(QItemSelection)));

    if (m_probe->needsObjectDiscovery()) {
        connect(m_probe->probe(), SIGNAL(objectCreated(QObject*)), SLOT(objectCreated(QObject*)));
        discoverObjects();
    }

    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)), this,
            SLOT(objectSelected(QObject*)));

    connect(m_remoteView, SIGNAL(elementsAtRequested(QPoint,GammaRay::RemoteViewInterface::RequestMode)), this, SLOT(requestElementsAt(QPoint,GammaRay::RemoteViewInterface::RequestMode)));
    connect(this, SIGNAL(elementsAtReceived(GammaRay::ObjectIds,int)), m_remoteView, SIGNAL(elementsAtReceived(GammaRay::ObjectIds,int)));
    connect(m_remoteView, SIGNAL(doPickElementId(GammaRay::ObjectId)), this, SLOT(pickElementId(GammaRay::ObjectId)));

    checkFeatures();
}

WidgetInspectorServer::~WidgetInspectorServer()
{
    disconnect(m_overlayWidget, SIGNAL(destroyed(QObject*)),
               this, SLOT(recreateOverlayWidget()));
    delete m_overlayWidget.data();
}

void WidgetInspectorServer::widgetSelected(const QItemSelection &selection)
{
    ProbeGuard guard;

    if (selection.isEmpty()) {
        m_propertyController->setObject(nullptr);
        return;
    }
    const QModelIndex index = selection.first().topLeft();

    QLayout *layout = nullptr;
    QWidget *widget = nullptr;
    if (index.isValid()) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        m_propertyController->setObject(obj);
        widget = qobject_cast<QWidget *>(obj);
        layout = qobject_cast<QLayout *>(obj);
        if (!widget && layout)
            widget = layout->parentWidget();
    } else {
        m_propertyController->setObject(nullptr);
    }

    if (m_selectedWidget == widget && !layout)
        return;

    if (!m_selectedWidget || !widget || m_selectedWidget->window() != widget->window())
        m_remoteView->resetView();
    m_selectedWidget = widget;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_remoteView->setEventReceiver(m_selectedWidget ? m_selectedWidget->window()->windowHandle() : Q_NULLPTR);
#endif

    if (m_selectedWidget
        && (qobject_cast<QDesktopWidget *>(m_selectedWidget)
            || m_selectedWidget->inherits("QDesktopScreenWidget"))) {
        m_overlayWidget->placeOn(WidgetOrLayoutFacade());
        return;
    }
    if (m_selectedWidget == m_overlayWidget) {
        // this should not happen, but apparently our object recovery is slightly too good sometimes ;)
        return;
    }

    if (layout)
        m_overlayWidget->placeOn(layout);
    else
        m_overlayWidget->placeOn(widget);

    if (!m_selectedWidget)
        return;

    updateWidgetPreview();
}

bool WidgetInspectorServer::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_selectedWidget && event->type() == QEvent::Paint)
        m_remoteView->sourceChanged();

    // make modal dialogs non-modal so that the gammaray window is still reachable
    // TODO: should only be done in in-process mode
    if (event->type() == QEvent::Show) {
        QDialog *dlg = qobject_cast<QDialog *>(object);
        if (dlg)
            dlg->setWindowModality(Qt::NonModal);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
        if (mouseEv->button() == Qt::LeftButton
            && mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
            QWidget *widget = QApplication::widgetAt(mouseEv->globalPos());
            if (widget) {
                // also select the corresponding model if a view was selected
                if (QAbstractItemView *view = Util::findParentOfType<QAbstractItemView>(object))
                    m_probe->selectObject(view->selectionModel());
                else if (QComboBox *box = Util::findParentOfType<QComboBox>(object))
                    m_probe->selectObject(box->model());
                else if (auto toolButton = qobject_cast<QToolButton*>(object)) {
                    if (toolButton->defaultAction())
                        m_probe->selectObject(toolButton->defaultAction());
                }

                m_probe->selectObject(widget, widget->mapFromGlobal(mouseEv->globalPos()));
                widgetSelected(widget);
            }
        }
    }

    return QObject::eventFilter(object, event);
}

void WidgetInspectorServer::updateWidgetPreview()
{
    if (!m_remoteView->isActive() || !m_selectedWidget)
        return;

    RemoteViewFrame frame;
    frame.setImage(imageForWidget(m_selectedWidget->window()));
    m_remoteView->sendFrame(frame);
}

void WidgetInspectorServer::requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode)
{
    if (!m_selectedWidget)
        return;
    auto window = m_selectedWidget->window();

    int bestCandidate;
    const ObjectIds objects = recursiveWidgetsAt(window, pos, mode, bestCandidate);

    if (!objects.isEmpty()) {
        emit elementsAtReceived(objects, bestCandidate);
    }
}

void WidgetInspectorServer::pickElementId(const GammaRay::ObjectId &id)
{
    QWidget *widget = id.asQObjectType<QWidget *>();
    if (widget)
        widgetSelected(widget);
}

QImage WidgetInspectorServer::imageForWidget(QWidget *widget)
{
    // prevent "recursion", i.e. infinite update loop, in our eventFilter
    Util::SetTempValue<QPointer<QWidget> > guard(m_selectedWidget, nullptr);
    // We should use hidpi rendering but it's buggy so let stay with
    // low dpi rendering. See QTBUG-53801
    const qreal ratio = 1; // widget->window()->devicePixelRatio();
    QImage img(widget->size() * ratio, QImage::Format_ARGB32);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    img.setDevicePixelRatio(ratio);
#endif
    img.fill(Qt::transparent);
    widget->render(&img);
    return img;
}

void WidgetInspectorServer::recreateOverlayWidget()
{
    ProbeGuard guard;
    m_overlayWidget = new OverlayWidget;
    m_overlayWidget->hide();

    // the target application might have destroyed the overlay widget
    // (e.g. because the parent of the overlay got destroyed).
    // just recreate a new one in this case
    connect(m_overlayWidget, SIGNAL(destroyed(QObject*)),
            this, SLOT(recreateOverlayWidget()));
}

void WidgetInspectorServer::widgetSelected(QWidget *widget)
{
    if (m_selectedWidget == widget)
        return;

    const QAbstractItemModel *model = m_widgetSelectionModel->model();
    const QModelIndexList indexList
        = model->match(model->index(0, 0),
                       ObjectModel::ObjectRole,
                       QVariant::fromValue<QObject *>(widget), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;
    const QModelIndex index = indexList.first();
    m_widgetSelectionModel->select(
        index,
        QItemSelectionModel::Select | QItemSelectionModel::Clear
        |QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void WidgetInspectorServer::objectSelected(QObject *obj)
{
    if (auto wdg = qobject_cast<QWidget *>(obj)) {
        widgetSelected(wdg);
    } else if (auto layout = qobject_cast<QLayout *>(obj)) {
        // TODO select the layout directly here
        if (layout->parentWidget())
            widgetSelected(layout->parentWidget());
    }
}

// TODO the following actions should actually store the file on the client!

void WidgetInspectorServer::saveAsImage(const QString &fileName)
{
    if (fileName.isEmpty() || !m_selectedWidget)
        return;

    m_overlayWidget->hide();
    QImage img = imageForWidget(m_selectedWidget);
    m_overlayWidget->show();
    img.save(fileName);
}

void WidgetInspectorServer::saveAsSvg(const QString &fileName)
{
    if (fileName.isEmpty() || !m_selectedWidget)
        return;

    m_overlayWidget->hide();
    callExternalExportAction("gammaray_save_widget_to_svg", m_selectedWidget, fileName);
    m_overlayWidget->show();
}

void WidgetInspectorServer::saveAsPdf(const QString &fileName)
{
    if (fileName.isEmpty() || !m_selectedWidget)
        return;

    m_overlayWidget->hide();
    callExternalExportAction("gammaray_save_widget_to_pdf", m_selectedWidget, fileName);
    m_overlayWidget->show();
}

void WidgetInspectorServer::saveAsUiFile(const QString &fileName)
{
    if (fileName.isEmpty() || !m_selectedWidget)
        return;

    callExternalExportAction("gammaray_save_widget_to_ui", m_selectedWidget, fileName);
}

GammaRay::ObjectIds WidgetInspectorServer::recursiveWidgetsAt(QWidget *parent, const QPoint &pos,
                                                              GammaRay::RemoteViewInterface::RequestMode mode, int &bestCandidate) const
{
    Q_ASSERT(parent);
    ObjectIds objects;

    bestCandidate = -1;

    const auto childItems = parent->children();
    for (int i = childItems.size() - 1; i >= 0; --i) { // backwards to match z order
        auto c = childItems.at(i);
        if (!c->isWidgetType() || c->metaObject()->className() == QLatin1String("GammaRay::OverlayWidget"))
            continue;
        auto w = qobject_cast<QWidget *>(c);
        const QPoint p = w->mapFromParent(pos);

        if (w->rect().contains(p, true)) {
            const bool hasSubChildren = !w->children().isEmpty();

            if (hasSubChildren) {
                const int count = objects.count();
                int bc;
                objects << recursiveWidgetsAt(w, p, mode, bc);

                if (bestCandidate == -1 && bc != -1) {
                    bestCandidate = count + bc;
                }
            }
            else {
                if (bestCandidate == -1 && isGoodCandidateWidget(w)) {
                    bestCandidate = objects.count();
                }

                objects << ObjectId(w);
            }
        }

        if (bestCandidate != -1 && mode == RemoteViewInterface::RequestBest) {
            break;
        }
    }

    if (bestCandidate == -1 && isGoodCandidateWidget(parent)) {
        bestCandidate = objects.count();
    }

    objects << ObjectId(parent);

    if (bestCandidate != -1 && mode == RemoteViewInterface::RequestBest) {
        objects = ObjectIds() << objects[bestCandidate];
        bestCandidate = 0;
    }

    return objects;
}

void WidgetInspectorServer::callExternalExportAction(const char *name, QWidget *widget,
                                                     const QString &fileName)
{
    if (!m_externalExportActions->isLoaded()) {
        foreach (const auto &path, Paths::pluginPaths(GAMMARAY_PROBE_ABI)) {
            m_externalExportActions->setFileName(path + QLatin1String("/libgammaray_widget_export_actions")
#if defined(GAMMARAY_INSTALL_QT_LAYOUT)
                + QStringLiteral("-") + QStringLiteral(GAMMARAY_PROBE_ABI)
#endif
            );
            if (m_externalExportActions->load())
                break;
        }
    }

    void (*function)(QWidget *, const QString &)
        = reinterpret_cast<void (*)(QWidget *,
                                    const QString &)>(m_externalExportActions->resolve(name));

    if (!function) {
        cerr << Q_FUNC_INFO << ' ' << qPrintable(m_externalExportActions->errorString()) << endl;
        return;
    }
    function(widget, fileName);
}

void WidgetInspectorServer::analyzePainting()
{
    if (!m_selectedWidget || !PaintAnalyzer::isAvailable())
        return;

    m_overlayWidget->hide();
    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(m_selectedWidget->rect());
    m_selectedWidget->render(m_paintAnalyzer->paintDevice());
    m_paintAnalyzer->endAnalyzePainting();
    m_overlayWidget->show();
}

void WidgetInspectorServer::checkFeatures()
{
    Features f = NoFeature;
#ifdef HAVE_QT_SVG
    f |= SvgExport;
#endif
#ifdef HAVE_QT_PRINTSUPPORT
    f |= PdfExport;
#endif
#ifdef HAVE_QT_DESIGNER
    f |= UiExport;
#endif
    if (PaintAnalyzer::isAvailable())
        f |= AnalyzePainting;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    f |= InputRedirection;
#endif
    setFeatures(f);
}

void WidgetInspectorServer::registerWidgetMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QLayoutItem)
    MO_ADD_PROPERTY(QLayoutItem, Qt::Alignment, alignment, setAlignment);
    MO_ADD_PROPERTY_RO(QLayoutItem, QSizePolicy::ControlTypes, controlTypes);
    MO_ADD_PROPERTY_RO(QLayoutItem, Qt::Orientations, expandingDirections);
    MO_ADD_PROPERTY_CR(QLayoutItem, QRect, geometry, setGeometry);
    MO_ADD_PROPERTY_RO(QLayoutItem, bool, hasHeightForWidth);
    MO_ADD_PROPERTY_RO(QLayoutItem, bool, isEmpty);
    MO_ADD_PROPERTY_RO(QLayoutItem, QSize, maximumSize);
    MO_ADD_PROPERTY_RO(QLayoutItem, QSize, minimumSize);
    MO_ADD_PROPERTY_RO(QLayoutItem, QSize, sizeHint);

    MO_ADD_METAOBJECT2(QLayout, QObject, QLayoutItem);
    MO_ADD_PROPERTY_CR(QLayout, QMargins, contentsMargins, setContentsMargins);
    MO_ADD_PROPERTY_RO(QLayout, QRect, contentsRect);
    MO_ADD_PROPERTY_RO(QLayout, int, count);
    MO_ADD_PROPERTY(QLayout, bool, isEnabled, setEnabled);
    MO_ADD_PROPERTY_RO(QLayout, QWidget *, menuBar);
    MO_ADD_PROPERTY_RO(QLayout, QWidget *, parentWidget);

    MO_ADD_METAOBJECT1(QGridLayout, QLayout);
    MO_ADD_PROPERTY_RO(QGridLayout, int, columnCount);
    MO_ADD_PROPERTY_RO(QGridLayout, int, rowCount);

    MO_ADD_METAOBJECT2(QWidget, QObject, QPaintDevice);
    MO_ADD_PROPERTY_RO(QWidget, QWidget*, focusProxy);
    MO_ADD_PROPERTY_RO(QWidget, bool, isWindow);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QWidget, QLayout*, layout);
#endif
    MO_ADD_PROPERTY_RO(QWidget, QWidget*, window);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QWidget, QWindow*, windowHandle);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    MO_ADD_PROPERTY   (QWidget, Qt::WindowFlags, windowFlags, setWindowFlags);
#endif

    MO_ADD_METAOBJECT1(QStyle, QObject);
    MO_ADD_PROPERTY_RO(QStyle, const QStyle *, proxy);
    MO_ADD_PROPERTY_RO(QStyle, QPalette, standardPalette);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_METAOBJECT1(QApplication, QGuiApplication);
    MO_ADD_PROPERTY_ST(QApplication, QWidget *, activeModalWidget);
    MO_ADD_PROPERTY_ST(QApplication, QWidget *, activePopupWidget);
    MO_ADD_PROPERTY_ST(QApplication, QWidget *, activeWindow);
    MO_ADD_PROPERTY_ST(QApplication, int, colorSpec);
    MO_ADD_PROPERTY_ST(QApplication, QDesktopWidget *, desktop);
    MO_ADD_PROPERTY_ST(QApplication, QWidget *, focusWidget);
    MO_ADD_PROPERTY_ST(QApplication, QStyle *, style);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_ST(QApplication, QWidgetList, topLevelWidgets);
#endif

    MO_ADD_METAOBJECT1(QFrame, QWidget);
    MO_ADD_METAOBJECT1(QAbstractScrollArea, QFrame);
    MO_ADD_METAOBJECT1(QAbstractItemView, QAbstractScrollArea);
    MO_ADD_PROPERTY_RO(QAbstractItemView, QAbstractItemModel *, model);

    MO_ADD_METAOBJECT1(QComboBox, QWidget);
    MO_ADD_PROPERTY_RO(QComboBox, QAbstractItemModel *, model);

    MO_ADD_METAOBJECT1(QAbstractButton, QWidget);
    MO_ADD_PROPERTY_RO(QAbstractButton, QButtonGroup*, group);

    MO_ADD_METAOBJECT1(QToolButton, QAbstractButton);
    MO_ADD_PROPERTY_RO(QToolButton, QAction*, defaultAction);
    MO_ADD_PROPERTY_RO(QToolButton, QMenu*, menu);
#endif

    MO_ADD_METAOBJECT0(QSizePolicy);
    MO_ADD_PROPERTY   (QSizePolicy, QSizePolicy::ControlType, controlType, setControlType);
    MO_ADD_PROPERTY_RO(QSizePolicy, Qt::Orientations, expandingDirections);
    MO_ADD_PROPERTY   (QSizePolicy, bool, hasHeightForWidth, setHeightForWidth);
    MO_ADD_PROPERTY   (QSizePolicy, bool, hasWidthForHeight, setWidthForHeight);
    MO_ADD_PROPERTY   (QSizePolicy, QSizePolicy::Policy, horizontalPolicy, setHorizontalPolicy);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QSizePolicy, int, horizontalStretch);
#else
    MO_ADD_PROPERTY   (QSizePolicy, int, horizontalStretch, setHorizontalStretch);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    MO_ADD_PROPERTY   (QSizePolicy, bool, retainSizeWhenHidden, setRetainSizeWhenHidden);
#endif
    MO_ADD_PROPERTY   (QSizePolicy, QSizePolicy::Policy, verticalPolicy, setVerticalPolicy);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QSizePolicy, int, verticalStretch);
#else
    MO_ADD_PROPERTY   (QSizePolicy, int, verticalStretch, setVerticalStretch);
#endif
}

static QString sizePolicyPolicyToString(QSizePolicy::Policy policy)
{
    const int index = QSizePolicy::staticMetaObject.indexOfEnumerator("Policy");
    const QMetaEnum metaEnum = QSizePolicy::staticMetaObject.enumerator(index);
    return QString::fromLatin1(metaEnum.valueToKey(policy));
}

static QString sizePolicyToString(const QSizePolicy &policy)
{
    return sizePolicyPolicyToString(policy.horizontalPolicy()) + " x "
           + sizePolicyPolicyToString(policy.verticalPolicy());
}

void WidgetInspectorServer::registerVariantHandlers()
{
    VariantHandler::registerStringConverter<QSizePolicy>(sizePolicyToString);
    VariantHandler::registerStringConverter<const QStyle*>(Util::displayString);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    VariantHandler::registerStringConverter<QWidget *>(Util::displayString);
#endif
}

void WidgetInspectorServer::discoverObjects()
{
    if (qApp) {
        foreach (QWidget *widget, qApp->topLevelWidgets())
            m_probe->discoverObject(widget);
    }
}

void WidgetInspectorServer::objectCreated(QObject *object)
{
    if (!object)
        return;

    if (qobject_cast<QApplication *>(object))
        discoverObjects();
    if (QAbstractItemView *view = qobject_cast<QAbstractItemView *>(object))
        m_probe->discoverObject(view->model());
}
