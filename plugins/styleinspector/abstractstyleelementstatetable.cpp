/*
  abstractstyleelementstatetable.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "abstractstyleelementstatetable.h"
#include "styleoption.h"
#include "styleinspectorinterface.h"
#include <common/objectbroker.h>

#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QApplication>

using namespace GammaRay;

AbstractStyleElementStateTable::AbstractStyleElementStateTable(QObject *parent)
    : AbstractStyleElementModel(parent)
    , m_interface(ObjectBroker::object<StyleInspectorInterface *>())
{
    connect(m_interface, &StyleInspectorInterface::cellSizeChanged, this, &AbstractStyleElementStateTable::cellSizeChanged);
}

void AbstractStyleElementStateTable::cellSizeChanged()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

int AbstractStyleElementStateTable::doColumnCount() const
{
    return StyleOption::stateCount();
}

QVariant AbstractStyleElementStateTable::doData(int row, int column, int role) const
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    if (role == Qt::SizeHintRole)
        return m_interface->cellSizeHint();
    return QVariant();
}

QVariant AbstractStyleElementStateTable::headerData(int section, Qt::Orientation orientation,
                                                    int role) const
{
    if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::ToolTipRole))
        return StyleOption::stateDisplayName(section);
    return QAbstractItemModel::headerData(section, orientation, role);
}

void AbstractStyleElementStateTable::fillStyleOption(QStyleOption *option, int column) const
{
    option->rect = QRect(0, 0, m_interface->cellWidth(), m_interface->cellHeight());
    option->palette = qApp->palette();
    option->state = StyleOption::prettyState(column);
}
