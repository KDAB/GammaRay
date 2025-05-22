/*
  searchlinecontroller.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "searchlinecontroller.h"

#include <QLineEdit>
#include <QRegularExpression>
#include <QTimer>
#include <QTreeView>
#include <QAbstractProxyModel>

#include "common/remotemodelroles.h"

using namespace GammaRay;

namespace {
QAbstractItemModel *findEffectiveFilterModel(QAbstractItemModel *model)
{
    Q_ASSERT(model);

    if (model->metaObject()->indexOfProperty("filterKeyColumn") != -1) {
        return model;
    }

    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(model);

    if (proxy) {
        return findEffectiveFilterModel(proxy->sourceModel());
    }

    return nullptr;
}
}

SearchLineController::SearchLineController(QLineEdit *lineEdit, QAbstractItemModel *proxyModel, QTreeView *treeView)
    : QObject(lineEdit)
    , m_lineEdit(lineEdit)
    , m_filterModel(findEffectiveFilterModel(proxyModel))
    , m_targetTreeView(treeView)
{
    Q_ASSERT(lineEdit);
    Q_ASSERT(m_filterModel);

    if (!m_filterModel) {
        QMetaObject::invokeMethod(this, "deleteLater", Qt::QueuedConnection);
        return;
    }

    m_filterModel->setProperty("filterKeyColumn", -1);
    m_filterModel->setProperty("filterCaseSensitivity", Qt::CaseInsensitive);
    activateSearch();

    m_lineEdit->setClearButtonEnabled(true);
    if (m_lineEdit->placeholderText().isEmpty())
        m_lineEdit->setPlaceholderText(tr("Search"));

    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(300);
    connect(lineEdit, &QLineEdit::textChanged, timer, [timer] { timer->start(); });
    connect(timer, &QTimer::timeout, this, [this] {
        activateSearch();
        QTimer::singleShot(50, this, [this] {
            onSearchFinished(m_lineEdit->text());
        });
    });
}

SearchLineController::~SearchLineController() = default;

void SearchLineController::activateSearch()
{
    if (m_filterModel) {
        m_filterModel->setProperty("filterRegularExpression",
                                   QRegularExpression(m_lineEdit->text(), QRegularExpression::CaseInsensitiveOption));
    }
}

void SearchLineController::onSearchFinished(const QString &searchTerm)
{
    if (!m_targetTreeView) {
        return;
    }

    if (searchTerm.isEmpty()) {
        // Make sure we keep the current item in view on clearing
        auto current = m_targetTreeView->currentIndex();
        if (current.isValid()) {
            m_targetTreeView->scrollTo(current);
        }
        return;
    }
    m_delayedIdxesToExpand.clear();

    if (!m_delayedExpandTimer) {
        m_delayedExpandTimer = new QTimer(this);
        m_delayedExpandTimer->setSingleShot(true);
        m_delayedExpandTimer->setInterval(125);

        connect(m_delayedExpandTimer, &QTimer::timeout, this, [this] {
            QVector<QPersistentModelIndex> stillNotLoaded;
            const auto copy = m_delayedIdxesToExpand;
            m_delayedIdxesToExpand.clear();
            auto it = copy.cbegin();
            auto end = copy.cend();
            for (; it != end; ++it) {
                const QModelIndex index = *it;
                if (!index.isValid()) {
                    continue;
                }
                if (m_targetTreeView->isExpanded(index)) {
                    continue;
                }
                const auto state = index.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>();
                if (!state.testFlag(RemoteModelNodeState::Empty)) {
                    expandRecursively(index);
                    continue;
                }
                QPersistentModelIndex notLoaded = index;
                stillNotLoaded.append(notLoaded);
            }

            m_delayedIdxesToExpand << stillNotLoaded;
            if (!m_delayedIdxesToExpand.isEmpty()) {
                m_delayedExpandTimer->start();
            }
        });
    }

    auto *model = m_targetTreeView->model();
    const int rowCount = model->rowCount({});
    // Walk the top level indexes and expand everything
    for (int r = 0; r < rowCount; ++r) {
        expandRecursively(model->index(r, 0));
    }
    // Start the timer to expand the not loaded indexes
    m_delayedExpandTimer->start();
}

void SearchLineController::expandRecursively(const QModelIndex &idx)
{
    if (!idx.isValid() || !m_filterModel || !m_targetTreeView) {
        return;
    }

    auto model = m_targetTreeView->model();
    m_targetTreeView->expand(idx);

    const int rowCount = model->rowCount(idx);
    for (int i = 0; i < rowCount; ++i) {
        auto childIdx = model->index(i, 0, idx);
        if (!childIdx.isValid()) {
            continue;
        }

        // The value might not be there, store it for delayed expansion
        const auto state = childIdx.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>();
        if (state & RemoteModelNodeState::Empty) {
            m_delayedIdxesToExpand << childIdx;
        } else {
            expandRecursively(childIdx);
        }
    }
}
