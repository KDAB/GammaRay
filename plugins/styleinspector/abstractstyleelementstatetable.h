/*
  abstractstyleelementstatetable.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTSTATETABLE_H
#define GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTSTATETABLE_H

#include "abstractstyleelementmodel.h"
#include <common/modelroles.h>

class QStyleOption;
class QRect;
class QPainter;

namespace GammaRay {

class StyleInspectorInterface;

/**
 * Base class for style element x style option state tables.
 * Covers the state part, sub-classes need to fill in the corresponding rows.
 */
class AbstractStyleElementStateTable : public GammaRay::AbstractStyleElementModel
{
  Q_OBJECT
  public:
    explicit AbstractStyleElementStateTable(QObject *parent = 0);

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

  protected:
    virtual int doColumnCount() const;
    virtual QVariant doData(int row, int column, int role) const;

    /// standard setup for the style option used in a cell in column @p column
    void fillStyleOption(QStyleOption *option, int column) const;

  protected:
    StyleInspectorInterface *m_interface;

  private slots:
    void cellSizeChanged();
};

}

#endif // GAMMARAY_ABSTRACTSTYLEELEMENTSTATETABLE_H
