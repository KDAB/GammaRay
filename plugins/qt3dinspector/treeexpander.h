/*
  treeexpander.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TREEEXPANDER_H
#define GAMMARAY_TREEEXPANDER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTreeView;
QT_END_NAMESPACE

namespace GammaRay {

class TreeExpander : public QObject
{
    Q_OBJECT
public:
    explicit TreeExpander(QTreeView *view);
    ~TreeExpander();

private:
    void rowsInserted(const QModelIndex &index, int start, int end);

    QTreeView *m_view;
};
}

#endif // GAMMARAY_TREEEXPANDER_H
