/*
  styleelementproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEELEMENTPROXYMODEL_H
#define GAMMARAY_STYLEELEMENTPROXYMODEL_H

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
#include <QSortFilterProxyModel>
typedef QSortFilterProxyModel QIdentityProxyModel;
#else
#include <QIdentityProxyModel>
#endif

#include <QSize>

namespace GammaRay {

class StyleElementProxyModel : public QIdentityProxyModel
{
  Q_OBJECT
  public:
    explicit StyleModelProxy(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;

  public slots:
    void setWidth(int width);
    void setHeight(int height);
    void setZoomFactor(int zoom);

  private:
    QSize m_sizeHint;
};

}

#endif // GAMMARAY_STYLEELEMENTPROXYMODEL_H
