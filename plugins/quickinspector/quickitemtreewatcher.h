/*
  quickitemtreewatcher.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMTREEWATCHER_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMTREEWATCHER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QTreeView;
QT_END_NAMESPACE

namespace GammaRay {
/** Auto-expand the visible item sub-tree. */
class QuickItemTreeWatcher : public QObject
{
    Q_OBJECT

public:
    explicit QuickItemTreeWatcher(QTreeView *itemView, QTreeView *sgView, QObject *parent = nullptr);
    ~QuickItemTreeWatcher() override;

private slots:
    void itemModelRowsInserted(const QModelIndex &parent, int start, int end);
    void sgModelRowsInserted(const QModelIndex &parent, int start, int end);

private:
    QTreeView *m_itemView;
    QTreeView *m_sgView;
};
}

#endif // GAMMARAY_QUICKITEMTREEWATCHER_H
