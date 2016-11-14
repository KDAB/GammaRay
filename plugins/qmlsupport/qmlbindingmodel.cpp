/*
  qmlbindingmodel.cpp

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

#include "qmlbindingmodel.h"

#include <QDebug>

#include <private/qqmlabstractbinding_p.h>
#include <private/qqmlbinding_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlproperty_p.h>

using namespace GammaRay;

QmlBindingModel::QmlBindingModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_obj(Q_NULLPTR)
{
}

QmlBindingModel::~QmlBindingModel()
{
}

void QmlBindingModel::setObject(QObject* obj)
{
    const auto bindings = bindingsFromObject(obj);

    // TODO use removerows/insertrows instead of reset here
    beginResetModel();
    m_obj = obj;
    m_bindings = bindings;
    endResetModel();
}

QVector<QmlBindingModel::BindingInfo> QmlBindingModel::bindingsFromObject(QObject* obj)
{
    QVector<BindingInfo> bindings;
    if (!obj)
        return bindings;

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
    if (!data)
        return bindings;

    auto b = data->bindings;
    while (b) {

        BindingInfo info;
        QQmlPropertyData::decodeValueTypePropertyIndex(b->targetPropertyIndex(), &info.propertyIndex);

        if (auto qmlBinding = dynamic_cast<QQmlBinding*>(b)) {
            info.sourceLocation = qmlBinding->expressionIdentifier();

#if 0
            auto context = QQmlEngine::contextForObject(obj);
            // manually try to get the code, QQmlBinding::expression() only gives us
            // literally "function() { [code] }"...
            QQmlEnginePrivate *ep = QQmlEnginePrivate::get(data->context->engine);
            QV4::Scope scope(ep->v4engine());
            QV4::ScopedValue f(scope, qmlBinding->m_function.value());
            QV4::Function *function = f->as<QV4::FunctionObject>()->function();

//             info.expression = function->code(QQmlEnginePrivate::getV4Engine(context->engine()), function->codeData);
            qDebug() << info.expression;
            qDebug() << function->codeData;
            qDebug() << function->codeData;
#endif
        }
        if (info.expression.isEmpty())
            info.expression = b->expression();
        bindings.push_back(info);
        b = b->nextBinding();
    }
#endif
    return bindings;
}


int QmlBindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int QmlBindingModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_bindings.size();
}

QVariant QmlBindingModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_obj)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &b = m_bindings.at(index.row());
        switch (index.column()) {
            case 0: {
                const auto prop = m_obj->metaObject()->property(b.propertyIndex);
                return prop.name();
            }
            case 1: return b.expression;
            case 2: return b.sourceLocation;
        }
    }

    return QVariant();
}

QVariant QmlBindingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Property");
            case 1: return tr("Expression");
            case 2: return tr("Source");
            case 3: return tr("Class");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
