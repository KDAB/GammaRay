/*
  pixelmetricmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_STYLEINSPECTOR_PIXELMETRICMODEL_H
#define GAMMARAY_STYLEINSPECTOR_PIXELMETRICMODEL_H

#include "abstractstyleelementmodel.h"

namespace GammaRay {

/**
 * Lists all pixel metric values of a given QStyle.
 */
class PixelMetricModel : public AbstractStyleElementModel
{
  Q_OBJECT
  public:
    explicit PixelMetricModel(QObject *parent = 0);

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

  protected:
    virtual QVariant doData(int row, int column, int role) const;
    virtual int doColumnCount() const;
    virtual int doRowCount() const;
};

}

#endif // GAMMARAY_PIXELMETRICMODEL_H
