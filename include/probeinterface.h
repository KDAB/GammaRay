/*
  probeinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the ProbeInterface abstract base class.

  @brief
  Declares the ProbeInterface abstract base class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_PROBEINTERFACE_H
#define GAMMARAY_PROBEINTERFACE_H

class QObject;
class QAbstractItemModel;

namespace GammaRay {

/**
 * @brief An abstract interface for accessing the core GammaRay probe.
 *
 * The ProbeInterface is an abstract interface that allows one to access
 * the core GammaRay probe without linking to it.
 */
class ProbeInterface
{
  public:
    virtual inline ~ProbeInterface()
    {
    }

    /**
     * Returns the object list model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    virtual QAbstractItemModel *objectListModel() const = 0;

    /**
     * Returns the object tree model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    virtual QAbstractItemModel *objectTreeModel() const = 0;

    /**
     * Returns the connection model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    virtual QAbstractItemModel *connectionModel() const = 0;

    /**
     * Determines if the specified QObject belongs to the GammaRay Probe or Window.
     *
     * These objects should not be tracked or shown to the user,
     * hence must be explictly filtered.
     * @param object is a pointer to a QObject instance.
     *
     * @return true if the specified QObject belongs to the GammaRay Probe
     * or Window; false otherwise.
     */
    virtual bool filterObject(QObject *object) const = 0;

    /**
     * Returns the probe QObject for connecting signals.
     * @return a pointer to a QObject instance.
     */
    virtual QObject *probe() const = 0;
};

}

#endif
