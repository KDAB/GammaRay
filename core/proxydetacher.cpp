/*
  proxydetacher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "proxydetacher.h"

#include <QWidget>
#include <QAbstractProxyModel>
#include <QHideEvent>
#include <QDebug>

using namespace GammaRay;

ProxyDetacher::ProxyDetacher(QWidget *widget,
                             QAbstractProxyModel *proxyModel,
                             QAbstractItemModel *sourceModel)
  : QObject(widget),
    m_widget(widget),
    m_proxy(proxyModel),
    m_source(sourceModel)
{
  widget->installEventFilter(this);
}

bool ProxyDetacher::eventFilter(QObject *obj, QEvent *e)
{
  Q_ASSERT(obj == m_widget);

  if (dynamic_cast<QHideEvent*>(e)) {
    Q_ASSERT(m_proxy->sourceModel() == m_source || !m_proxy->sourceModel());
    m_proxy->setSourceModel(0);
  } else if (dynamic_cast<QShowEvent*>(e)) {
    Q_ASSERT(!m_proxy->sourceModel());
    m_proxy->setSourceModel(m_source);
  }

  return QObject::eventFilter(obj, e);
}

