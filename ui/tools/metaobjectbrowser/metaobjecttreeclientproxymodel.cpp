/*
  metaobjecttreeclientproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "metaobjecttreeclientproxymodel.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QPalette>

using namespace GammaRay;

MetaObjectTreeClientProxyModel::MetaObjectTreeClientProxyModel(QObject* parent) :
    QIdentityProxyModel(parent)
{
}

MetaObjectTreeClientProxyModel::~MetaObjectTreeClientProxyModel()
{
}

void MetaObjectTreeClientProxyModel::setSourceModel(QAbstractItemModel* source)
{
    QIdentityProxyModel::setSourceModel(source);

    connect(source, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(findQObjectIndex()));
    connect(source, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(findQObjectIndex()));
}

// 1 / GRADIENT_SCALE_FACTOR is yellow, 2 / GRADIENT_SCALE_FACTOR and beyond is red
static const int GRADIENT_SCALE_FACTOR = 4;

static QColor colorForRatio(double ratio)
{
    const auto red = qBound(qreal(0.0), ratio * GRADIENT_SCALE_FACTOR, qreal(0.5));
    const auto green = qBound(qreal(0.0), 1 - ratio * GRADIENT_SCALE_FACTOR, qreal(0.5));
    auto color = QColor(255 * red, 255 * green, 0);
    if (QApplication::palette().color(QPalette::Base).lightness() > 128)
        return color.lighter(300);
    return color;
}

QVariant MetaObjectTreeClientProxyModel::data(const QModelIndex& index, int role) const
{
    if (!sourceModel())
        return QVariant();

    if ((role != Qt::BackgroundRole && role != Qt::ToolTipRole) || index.column() == 0 || !m_qobjIndex.isValid())
        return QIdentityProxyModel::data(index, role);

    if (!index.parent().isValid() && (index.row() != m_qobjIndex.row() || (index.row() == m_qobjIndex.row() && index.column() == 2)))
        return QIdentityProxyModel::data(index, role); // top-level but not QObject, or QObject incl count

    const auto count = index.data(Qt::DisplayRole).toInt();
    if (count <= 0)
        return QIdentityProxyModel::data(index, role);

    const auto totalCount = m_qobjIndex.sibling(m_qobjIndex.row(), 2).data().toInt();
    const auto ratio = (double)count / (double)totalCount;

    // at this point, role can only be background or tooltip

    if (role == Qt::BackgroundRole) {
        return colorForRatio(ratio);
    }

    Q_ASSERT(role == Qt::ToolTipRole);
    return tr("%1%").arg(ratio * 100.0, 0, 'f', 2);
}

void MetaObjectTreeClientProxyModel::findQObjectIndex()
{
    auto idxList = match(index(0, 0), Qt::DisplayRole, QStringLiteral("QObject"), 1, Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (idxList.isEmpty())
        return;

    m_qobjIndex = idxList.first();
    disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(findQObjectIndex()));
    disconnect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(findQObjectIndex()));
}
