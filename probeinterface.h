/*
  probeinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROBEINTERFACE_H
#define GAMMARAY_PROBEINTERFACE_H

class QObject;
class QAbstractItemModel;

namespace GammaRay
{

/**
 * Abstract interface for accessing the core probe without linking to it.
 */
class ProbeInterface
{
  public:
    virtual inline ~ProbeInterface() {}

    /** Returns the object list model. */
    virtual QAbstractItemModel *objectListModel() const = 0;
    /** Returns the object tree model. */
    virtual QAbstractItemModel *objectTreeModel() const = 0;
    /** Returns the connection model. */
    virtual QAbstractItemModel *connectionModel() const = 0;

    /** Returns the probe QObject for connecting signals. */
    virtual QObject *probe() const = 0;
};

}

#endif
