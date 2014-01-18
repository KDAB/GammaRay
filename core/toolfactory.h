/*
  toolfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
  This file is part of the GammaRay Plugin API and declares the ToolFactory abstract base class.

  @brief
  Declares the ToolFactory abstract base class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_TOOLFACTORY_H
#define GAMMARAY_TOOLFACTORY_H

#include "probeinterface.h"
#include <ui/tooluifactory.h>

#include <QMetaType>
#include <QStringList>
#include <QtPlugin>

namespace GammaRay {

class ProbeInterface;

/**
 * @brief An abstract interface for probe tools.
 *
 * The ToolFactory class is an abstract base class for creating probe tools
 * for GammaRay.  Each tool must have a unique identifier.
 */
class ToolFactory
{
  public:
    virtual inline ~ToolFactory()
    {
    }

    /**
     * Unique id of this tool
     * @return a QString containing the tool id.
     */
    virtual QString id() const = 0;

    /**
     * Human readable name of this tool.
     * @return a QString containing the tool name.
     */
    virtual QString name() const = 0;

    /**
     * Class names of types this tool can handle.
     * The tool will only be activated if an object of one of these types
     * is seen in the probed application.
     * @return a QStringList of class names of types this tool supports.
     */
    virtual QStringList supportedTypes() const = 0;

    /**
     * Initialize the tool.
     * Implement this method to do non-GUI initialization, such as creating
     * object tracking models etc.
     * @param probe The probe interface allowing access to the object models.
     */
    virtual void init(ProbeInterface *probe) = 0;

    /**
     * Allows to hide a plug-in from the UI.
     * This is useful for plug-ins that only provide support for additional
     * data types.
     * @since 2.1
     */
    virtual bool isHidden() const = 0;
};

/**
 * @brief A templated generic ToolFactory for some data type and tool.
 */
template <typename Type, typename Tool>
class StandardToolFactory : public ToolFactory
{
public:
  virtual inline QStringList supportedTypes() const
  {
    return QStringList(Type::staticMetaObject.className());
  }

  virtual inline QString id() const
  {
    return Tool::staticMetaObject.className();
  }

  virtual inline void init(ProbeInterface *probe)
  {
    new Tool(probe, probe->probe());
  }

  inline bool isHidden() const
  {
    return false;
  }
};

/**
 * @brief A templated generic ToolFactory for some data type and Tool.
 */
// TODO: split this completely, this is just a migration aid
template <typename Type, typename Tool, typename ToolUi>
class StandardToolFactory2 : public ToolFactory, public ToolUiFactory
{
  public:
    virtual inline QStringList supportedTypes() const
    {
      return QStringList(Type::staticMetaObject.className());
    }

    virtual inline QString id() const
    {
      return Tool::staticMetaObject.className();
    }

    virtual inline void init(ProbeInterface *probe)
    {
      new Tool(probe, probe->probe());
    }

    inline bool isHidden() const
    {
      return false;
    }

    virtual inline QWidget *createWidget(QWidget *parentWidget)
    {
      return new ToolUi(parentWidget);
    }

    virtual bool remotingSupported() const
    {
      return true;
    }
};

}

Q_DECLARE_INTERFACE(GammaRay::ToolFactory, "com.kdab.GammaRay.ToolFactory/1.0")
Q_DECLARE_METATYPE(GammaRay::ToolFactory *)

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#define Q_PLUGIN_METADATA(x)
#endif

#endif
