/*
  treeexpander.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
