/*
  proxydetacher.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROXYDETACHER_H
#define GAMMARAY_PROXYDETACHER_H

#include <QObject>

class QWidget;
class QAbstractItemModel;
class QAbstractProxyModel;

namespace GammaRay {

/**
 * Optimization Helper
 *
 * Detaches source model of proxy when widget gets hidden.
 * Re-attaches source model when widget gets shown again.
 */
class ProxyDetacher : public QObject
{
  Q_OBJECT
  public:
    explicit ProxyDetacher(QWidget *widget,
                           QAbstractProxyModel *proxyModel,
                           QAbstractItemModel *sourceModel);
    virtual bool eventFilter(QObject *obj, QEvent *e);

  private:
    QWidget *m_widget;
    QAbstractProxyModel *m_proxy;
    QAbstractItemModel *m_source;
};

}

#endif // PROXYDETACHER_H
