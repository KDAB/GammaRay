/*
  stacktracemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "stacktracemodel.h"

#include <QDebug>

using namespace GammaRay;

StackTraceModel::StackTraceModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

StackTraceModel::~StackTraceModel()
{
}

void StackTraceModel::setStackTrace(const Execution::Trace& trace)
{
    setStackTrace(Execution::resolveAll(trace));
}

void StackTraceModel::setStackTrace(const QVector<Execution::ResolvedFrame> &frames)
{
    if (!m_frames.empty()) {
        beginRemoveRows(QModelIndex(), 0, m_frames.size() - 1);
        m_frames.clear();
        endRemoveRows();
    }

    if (!frames.empty()) {
        beginInsertRows(QModelIndex(), 0, frames.size() - 1);
        m_frames = frames;
        endInsertRows();
    }
}

int StackTraceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int StackTraceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_frames.size();
}

QVariant StackTraceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return m_frames.at(index.row()).name;
            case 1: return QVariant::fromValue(m_frames.at(index.row()).location);
        }
    }

    return QVariant();
}

QVariant StackTraceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Function");
            case 1: return tr("Location");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
