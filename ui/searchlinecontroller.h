/*
  searchlinecontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SEARCHLINECONTROLLER_H
#define GAMMARAY_SEARCHLINECONTROLLER_H

#include "gammaray_ui_export.h"

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTimer;
class QTreeView;
QT_END_NAMESPACE

namespace GammaRay {
/** Couples a line edit to a QSortFilterProxyModel api-like model for as-you-type filtering.
 * The model don't really need to be a QSortFilterProxyModel, it can be a plain QAbstractItemModel
 * implementing QSFPM like properties (ie, filterKeyColumn, filterCaseSensitivity etc...).
 * If the given proxy is not a QSFPM api-like model, a check is performed recursively in all
 * sourceModel until a compatible QSFPM api-like one is found.
 *
 * If a treeView is provided, then the controller tries to auto expand the tree to reveal
 * matching indexes
 */
class GAMMARAY_UI_EXPORT SearchLineController : public QObject
{
    Q_OBJECT
public:
    /** Establish a connection between @p lineEdit and @p proxyModel. */
    explicit SearchLineController(QLineEdit *lineEdit, QAbstractItemModel *proxyModel, QTreeView *treeView = nullptr);
    ~SearchLineController() override;

private slots:
    void activateSearch();
    void onSearchFinished(const QString &searchTerm);

private:
    void expandRecursively(const QModelIndex &);

    QLineEdit *m_lineEdit;
    QPointer<QAbstractItemModel> m_filterModel;
    QPointer<QTreeView> m_targetTreeView;
    QTimer *m_delayedExpandTimer = nullptr;
    QVector<QPersistentModelIndex> m_delayedIdxesToExpand;
};
}

#endif // GAMMARAY_SEARCHLINECONTROLLER_H
