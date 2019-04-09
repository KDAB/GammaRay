/*
  loggingcategorymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "loggingcategorymodel.h"

using namespace GammaRay;

namespace GammaRay {
// FIXME: can be called from different threads!
void categoryFilter(QLoggingCategory *category)
{
    Q_ASSERT(LoggingCategoryModel::m_instance);
    LoggingCategoryModel::m_instance->addCategory(category);
    if (LoggingCategoryModel::m_instance->m_previousFilter)
        LoggingCategoryModel::m_instance->m_previousFilter(category);
}
}

LoggingCategoryModel *LoggingCategoryModel::m_instance = nullptr;

LoggingCategoryModel::LoggingCategoryModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_previousFilter(nullptr)
{
    Q_ASSERT(m_instance == nullptr);
    m_instance = this;
    m_previousFilter = QLoggingCategory::installFilter(categoryFilter);
}

LoggingCategoryModel::~LoggingCategoryModel()
{
    m_instance = nullptr;
    QLoggingCategory::installFilter(m_previousFilter);
}

void LoggingCategoryModel::addCategory(QLoggingCategory *category)
{
    beginInsertRows(QModelIndex(), m_categories.size(), m_categories.size());
    m_categories.push_back(category);
    endInsertRows();
}

int LoggingCategoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_categories.size();
}

int LoggingCategoryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant LoggingCategoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole && index.column() == 0)
        return QString::fromUtf8(m_categories.at(index.row())->categoryName());

    if (role == Qt::CheckStateRole) {
        auto cat = m_categories.at(index.row());
        switch (index.column()) {
        case 1:
            return cat->isDebugEnabled() ? Qt::Checked : Qt::Unchecked;
        case 2:
            return cat->isInfoEnabled() ? Qt::Checked : Qt::Unchecked;
        case 3:
            return cat->isWarningEnabled() ? Qt::Checked : Qt::Unchecked;
        case 4:
            return cat->isCriticalEnabled() ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

Qt::ItemFlags LoggingCategoryModel::flags(const QModelIndex &index) const
{
    const auto baseFlags = QAbstractTableModel::flags(index);
    if (index.column() == 2) // info not available in Qt < 5.5
        return baseFlags;
    if (index.column() > 0)
        return baseFlags | Qt::ItemIsUserCheckable;
    return baseFlags;
}

bool LoggingCategoryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() == 0 || role != Qt::CheckStateRole)
        return false;

    static const QtMsgType type_map[]
        = { QtDebugMsg, QtDebugMsg, QtInfoMsg, QtWarningMsg, QtCriticalMsg };

    const auto enabled = value.toInt() == Qt::Checked;
    auto cat = m_categories.at(index.row());
    cat->setEnabled(type_map[index.column()], enabled);
    emit dataChanged(index, index);
    return true;
}

QVariant LoggingCategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Category");
        case 1:
            return tr("Debug");
        case 2:
            return tr("Info");
        case 3:
            return tr("Warning");
        case 4:
            return tr("Critical");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
