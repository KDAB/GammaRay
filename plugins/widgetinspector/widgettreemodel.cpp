/*
  widgettreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "widgettreemodel.h"
#include "common/modelutils.h"
#include "widgetmodelroles.h"

#include <QLayout>
#include <QWidget>

using namespace GammaRay;

static bool isMainWindowSubclassAcceptor(const QVariant &v)
{
    const QObject *object = v.value<QObject *>();
    return object && object->inherits("QMainWindow");
}

WidgetTreeModel::WidgetTreeModel(QObject *parent)
    : ObjectFilterProxyModelBase(parent)
{
}

QPair<int, QVariant> WidgetTreeModel::defaultSelectedItem()
{
    // select the first QMainwindow window (if any) in the widget model
    return QPair<int, QVariant>(ObjectModel::ObjectRole,
                                QVariant::fromValue(&isMainWindowSubclassAcceptor));
}

QVariant WidgetTreeModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == WidgetModelRoles::WidgetFlags) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        QWidget *widget = qobject_cast<QWidget *>(obj);
        if (!widget) {
            QLayout *layout = qobject_cast<QLayout *>(obj);
            if (layout)
                widget = layout->parentWidget();
        }

        if (widget && !widget->isVisible())
            return WidgetModelRoles::Invisible;

        return WidgetModelRoles::None;
    }
    return QSortFilterProxyModel::data(index, role);
}

QMap<int, QVariant> WidgetTreeModel::itemData(const QModelIndex &index) const
{
    auto d = ObjectFilterProxyModelBase::itemData(index);
    d.insert(WidgetModelRoles::WidgetFlags, data(index, WidgetModelRoles::WidgetFlags));
    return d;
}

bool WidgetTreeModel::filterAcceptsObject(QObject *object) const
{
    return object->isWidgetType() || qobject_cast<QLayout *>(object);
}
