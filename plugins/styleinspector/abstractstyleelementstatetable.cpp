/*
  abstractstyleelementstatetable.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
