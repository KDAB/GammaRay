/*
  loggingcategorymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "loggingcategorymodel.h"
#include <QFile>

using namespace GammaRay;

namespace GammaRay {
void categoryFilter(QLoggingCategory *category)
{
    Q_ASSERT(LoggingCategoryModel::m_instance);
    Q_EMIT LoggingCategoryModel::m_instance->addCategorySignal(category);
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
    connect(this, &LoggingCategoryModel::addCategorySignal, this, &LoggingCategoryModel::addCategorySlot, Qt::QueuedConnection);
    m_previousFilter = QLoggingCategory::installFilter(categoryFilter);
}

LoggingCategoryModel::~LoggingCategoryModel()
{
    m_instance = nullptr;
    QLoggingCategory::installFilter(m_previousFilter);
}

QByteArray LoggingCategoryModel::exportLoggingConfig(bool all, bool forFile)
{
    QByteArray ret;
    if (forFile) {
        ret.append("[Rules]\n");
    }

    const char delimiter = forFile ? '\n' : ';';
    for (const auto &cat : qAsConst(m_categories)) {
        if (all || cat.category->isDebugEnabled() != cat.wasDebugEnabled) {
            ret.append(cat.category->categoryName());
            ret.append(cat.category->isDebugEnabled() ? ".debug=true" : ".debug=false");
            ret.append(delimiter);
        }

        if (all || cat.category->isInfoEnabled() != cat.wasInfoEnabled) {
            ret.append(cat.category->categoryName());
            ret.append(cat.category->isInfoEnabled() ? ".info=true" : ".info=false");
            ret.append(delimiter);
        }

        if (all || cat.category->isWarningEnabled() != cat.wasWarningEnabled) {
            ret.append(cat.category->categoryName());
            ret.append(cat.category->isWarningEnabled() ? ".warning=true" : ".warning=false");
            ret.append(delimiter);
        }

        if (all || cat.category->isCriticalEnabled() != cat.wasCriticalEnabled) {
            ret.append(cat.category->categoryName());
            ret.append(cat.category->isCriticalEnabled() ? ".critical=true" : ".critical=false");
            ret.append(delimiter);
        }
    }
    return ret;
}

void LoggingCategoryModel::addCategory(QLoggingCategory *category)
{
    beginInsertRows(QModelIndex(), m_categories.size(), m_categories.size());
    m_categories.push_back(CategoryWithDefaultValues {
        category,
        category->isDebugEnabled(),
        category->isInfoEnabled(),
        category->isWarningEnabled(),
        category->isCriticalEnabled() });
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
        return QString::fromUtf8(m_categories.at(index.row()).category->categoryName());

    if (role == Qt::CheckStateRole) {
        auto cat = m_categories.at(index.row());
        switch (index.column()) {
        case 1:
            return cat.category->isDebugEnabled() ? Qt::Checked : Qt::Unchecked;
        case 2:
            return cat.category->isInfoEnabled() ? Qt::Checked : Qt::Unchecked;
        case 3:
            return cat.category->isWarningEnabled() ? Qt::Checked : Qt::Unchecked;
        case 4:
            return cat.category->isCriticalEnabled() ? Qt::Checked : Qt::Unchecked;
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

    static const QtMsgType type_map[] = { QtDebugMsg, QtDebugMsg, QtInfoMsg, QtWarningMsg, QtCriticalMsg };

    const auto enabled = value.toInt() == Qt::Checked;
    auto cat = m_categories.at(index.row());
    cat.category->setEnabled(type_map[index.column()], enabled);
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

void LoggingCategoryModel::addCategorySlot(QLoggingCategory *category)
{
    addCategory(category);
}
