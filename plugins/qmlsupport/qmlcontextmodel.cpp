/*
  qmlcontextmodel.cpp

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
    m_contexts = tmp;
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
            if (context->baseUrl().scheme() == QLatin1String("file"))     // ### use SourceLocation for this!
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
