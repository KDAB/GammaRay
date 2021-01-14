/*
  styleelementproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEELEMENTPROXYMODEL_H
#define GAMMARAY_STYLEINSPECTOR_STYLEELEMENTPROXYMODEL_H

#include <QIdentityProxyModel>

#include <QSize>

namespace GammaRay {
class StyleElementProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit StyleModelProxy(QObject *parent = nullptr);

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
