/*
  widgettreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "widgettreemodel.h"
#include "common/modelutils.h"

#include <QApplication>
#include <QLayout>
#include <QPalette>
#include <QWidget>

using namespace GammaRay;

static bool isMainWindowSubclassAcceptor(const QVariant &v)
{
  const QObject *object = v.value<QObject*>();
  return object && object->inherits("QMainWindow");
}

WidgetTreeModel::WidgetTreeModel(QObject *parent)
  : ObjectFilterProxyModelBase(parent)
{
}

QPair<int, QVariant> WidgetTreeModel::defaultSelectedItem() const
{
  // select the first QMainwindow window (if any) in the widget model
  return QPair<int, QVariant>(ObjectModel::ObjectRole, QVariant::fromValue(&isMainWindowSubclassAcceptor));
}

QVariant WidgetTreeModel::data(const QModelIndex &index, int role) const
{
  if (index.isValid() && role == Qt::ForegroundRole) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget) {
      QLayout *layout = qobject_cast<QLayout*>(obj);
      if (layout) {
        widget = layout->parentWidget();
      }
    }
    if (widget && !widget->isVisible()) {
      return qApp->palette().color(QPalette::Disabled, QPalette::Text);
    }
  }
  return QSortFilterProxyModel::data(index, role);
}

bool WidgetTreeModel::filterAcceptsObject(QObject *object) const
{
  return object->isWidgetType() || qobject_cast<QLayout*>(object);
}
