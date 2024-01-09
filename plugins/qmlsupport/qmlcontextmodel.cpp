/*
  qmlcontextmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qmlcontextmodel.h"

#include <core/util.h>
#include <common/objectmodel.h>

#include <QQmlContext>

#include <algorithm>

using namespace GammaRay;

QmlContextModel::QmlContextModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QmlContextModel::~QmlContextModel() = default;

void QmlContextModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    m_contexts.clear();
    endRemoveRows();
}

void QmlContextModel::setContext(QQmlContext *leafContext)
{
    if (!m_contexts.isEmpty()) {
        if (m_contexts.last() != leafContext)
            clear();
        else
            return;
    }

    if (!leafContext)
        return;

    Q_ASSERT(m_contexts.isEmpty());
    QVector<QQmlContext *> tmp;
    auto context = leafContext;
    do {
        tmp.push_back(context);
        context = context->parentContext();
    } while (context);
    std::reverse(tmp.begin(), tmp.end());

    beginInsertRows(QModelIndex(), 0, tmp.size() - 1);
    m_contexts = std::move(tmp);
    endInsertRows();
}

int QmlContextModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int QmlContextModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_contexts.size();
}

QVariant QmlContextModel::data(const QModelIndex &index, int role) const
{
    if (m_contexts.isEmpty() || !index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        auto context = m_contexts.at(index.row());
        switch (index.column()) {
        case 0:
            return Util::shortDisplayString(context);
        case 1:
            if (context->baseUrl().scheme() == QLatin1String("file")) // ### use SourceLocation for this!
                return context->baseUrl().path();
            return context->baseUrl().toString();
        }
    } else if (role == ObjectModel::ObjectRole) {
        return QVariant::fromValue(m_contexts.at(index.row()));
    }

    return QVariant();
}

QVariant QmlContextModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Context");
        case 1:
            return tr("Location");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
