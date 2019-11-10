/*
  metaobjecttreeclientproxymodel.cpp

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

#include "metaobjecttreeclientproxymodel.h"

#include <ui/uiintegration.h>

#include <common/qmetaobjectvalidatorresult.h>
#include <common/tools/metaobjectbrowser/qmetaobjectmodel.h>

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QStyle>

using namespace GammaRay;

MetaObjectTreeClientProxyModel::MetaObjectTreeClientProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

MetaObjectTreeClientProxyModel::~MetaObjectTreeClientProxyModel() = default;

void MetaObjectTreeClientProxyModel::setSourceModel(QAbstractItemModel *source)
{
    QIdentityProxyModel::setSourceModel(source);

    connect(source, &QAbstractItemModel::rowsInserted, this, &MetaObjectTreeClientProxyModel::findQObjectIndex);
    connect(source, &QAbstractItemModel::dataChanged, this, &MetaObjectTreeClientProxyModel::findQObjectIndex);
    findQObjectIndex();
}

// 1 / GRADIENT_SCALE_FACTOR is yellow, 2 / GRADIENT_SCALE_FACTOR and beyond is red
static const int GRADIENT_SCALE_FACTOR = 4;

static QColor colorForRatio(double ratio)
{
    const auto red = qBound<qreal>(0.0, ratio * GRADIENT_SCALE_FACTOR, 0.5);
    const auto green = qBound<qreal>(0.0, 1 - ratio * GRADIENT_SCALE_FACTOR, 0.5);
    auto color = QColor(255 * red, 255 * green, 0);
    if (!UiIntegration::hasDarkUI())
        return color.lighter(300);
    return color;
}

static QString issuesToString(QMetaObjectValidatorResult::Results r)
{
    QStringList l;
    if (r & QMetaObjectValidatorResult::SignalOverride)
        l.push_back(MetaObjectTreeClientProxyModel::tr("overrides base class signal"));
    if (r & QMetaObjectValidatorResult::UnknownMethodParameterType)
        l.push_back(MetaObjectTreeClientProxyModel::tr("method uses parameter type not registered with the meta type system"));
    if (r & QMetaObjectValidatorResult::PropertyOverride)
        l.push_back(MetaObjectTreeClientProxyModel::tr("overrides base class property"));
    if (r & QMetaObjectValidatorResult::UnknownPropertyType)
        l.push_back(MetaObjectTreeClientProxyModel::tr("property with type not registered with the meta type system"));
    return MetaObjectTreeClientProxyModel::tr("Issues:<ul><li>%1</li></ul>").arg(l.join("</li><li>"));
}

QVariant MetaObjectTreeClientProxyModel::data(const QModelIndex &index, int role) const
{
    if (!sourceModel() || !index.isValid())
        return QVariant();

    if (index.column() == QMetaObjectModel::ObjectColumn) {
        const auto issues = QIdentityProxyModel::data(index, QMetaObjectModel::MetaObjectIssues).value<QMetaObjectValidatorResult::Results>();
        switch (role) {
            case Qt::DecorationRole:
                if (issues != QMetaObjectValidatorResult::NoIssue)
                    return qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning);
                break;
            case Qt::ToolTipRole:
            {
                if (issues != QMetaObjectValidatorResult::NoIssue)
                    return issuesToString(issues);
                const auto invalid = index.sibling(index.row(), QMetaObjectModel::ObjectInclusiveAliveCountColumn).data(QMetaObjectModel::MetaObjectInvalid).toBool();
                if (invalid)
                    return tr("This meta object might have been deleted.");
                break;
            }
        }
        return QIdentityProxyModel::data(index, role);
    }

    if ((role != Qt::BackgroundRole && role != Qt::ToolTipRole) || !m_qobjIndex.isValid())
        return QIdentityProxyModel::data(index, role);

    if (!needsBackground(index))
        return QIdentityProxyModel::data(index, role); // top-level but not QObject, or QObject incl count

    const auto count = index.data(Qt::DisplayRole).toInt();
    if (count <= 0)
        return QIdentityProxyModel::data(index, role);

    const auto totalColumn = (index.column() == QMetaObjectModel::ObjectSelfCountColumn || index.column() == QMetaObjectModel::ObjectInclusiveCountColumn)?
        QMetaObjectModel::ObjectInclusiveCountColumn : QMetaObjectModel::ObjectInclusiveAliveCountColumn;
    const auto totalCount = m_qobjIndex.sibling(m_qobjIndex.row(), totalColumn).data().toInt();
    const auto ratio = (double)count / (double)totalCount;

    // at this point, role can only be background or tooltip

    if (role == Qt::BackgroundRole)
        return colorForRatio(ratio);

    Q_ASSERT(role == Qt::ToolTipRole);
    return tr("%1%").arg(ratio * 100.0, 0, 'f', 2);
}

QVariant MetaObjectTreeClientProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case QMetaObjectModel::ObjectColumn:
                return tr("Meta Object Class");
            case QMetaObjectModel::ObjectSelfCountColumn:
                return tr("Self Total");
            case QMetaObjectModel::ObjectInclusiveCountColumn:
                return tr("Incl. Total");
            case QMetaObjectModel::ObjectSelfAliveCountColumn:
                return tr("Self Alive");
            case QMetaObjectModel::ObjectInclusiveAliveCountColumn:
                return tr("Incl. Alive");
            default:
                return QVariant();
        }
    } else if (role == Qt::ToolTipRole) {
        switch (section) {
            case QMetaObjectModel::ObjectColumn:
                return tr("This column shows the QMetaObject class hierarchy.");
            case QMetaObjectModel::ObjectSelfCountColumn:
                return tr("This column shows the number of objects created of a particular type.");
            case QMetaObjectModel::ObjectInclusiveCountColumn:
                return tr("This column shows the number of objects created that inherit from a particular type.");
            case QMetaObjectModel::ObjectSelfAliveCountColumn:
                return tr("This column shows the number of objects created and not yet destroyed of a particular type.");
            case QMetaObjectModel::ObjectInclusiveAliveCountColumn:
                return tr("This column shows the number of objects created and not yet destroyed that inherit from a particular type.");
            default:
                return QVariant();
        }
    }

    return QIdentityProxyModel::headerData(section, orientation, role);
}

Qt::ItemFlags MetaObjectTreeClientProxyModel::flags(const QModelIndex& index) const
{
    auto f = QIdentityProxyModel::flags(index);
    if (index.isValid()) {
        const auto idx = index.sibling(index.row(), QMetaObjectModel::ObjectInclusiveAliveCountColumn);
        const auto invalid = idx.data(QMetaObjectModel::MetaObjectInvalid).toBool();
        if (invalid)
            f &= ~Qt::ItemIsEnabled;
    }
    return f;
}

void MetaObjectTreeClientProxyModel::findQObjectIndex()
{
    auto idxList = match(index(0, 0), Qt::DisplayRole,
                         QStringLiteral("QObject"), 1,
                         Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (idxList.isEmpty())
        return;

    m_qobjIndex = idxList.first();
    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this,
               &MetaObjectTreeClientProxyModel::findQObjectIndex);
    disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this,
               &MetaObjectTreeClientProxyModel::findQObjectIndex);
}

bool MetaObjectTreeClientProxyModel::needsBackground(const QModelIndex &index) const
{
    if (index.parent().isValid())
        return true;
    if (index.row() != m_qobjIndex.row())
        return true;
    if (index.column() == QMetaObjectModel::ObjectInclusiveCountColumn || index.column() == QMetaObjectModel::ObjectInclusiveAliveCountColumn)
        return false;
    return true;
}
