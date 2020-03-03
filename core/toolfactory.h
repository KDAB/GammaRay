/*
  toolfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the ToolFactory abstract base class.

  @brief
  Declares the ToolFactory abstract base class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_TOOLFACTORY_H
#define GAMMARAY_TOOLFACTORY_H

#include "gammaray_core_export.h"
#include "probe.h"

#include <QMetaType>
#include <QStringList>
#include <QtPlugin>
#include <QVector>

namespace GammaRay {

/*!
 * An abstract interface for probe tools.
 *
 * The ToolFactory class is an abstract base class for creating probe tools
 * for GammaRay.  Each tool must have a unique identifier.
 */
class GAMMARAY_CORE_EXPORT ToolFactory
{
public:
    ToolFactory();
    virtual ~ToolFactory();

    /*!
     * Unique id of this tool
     * @return a QString containing the tool id.
     */
    virtual QString id() const = 0;

    /*!
     * Class names of types this tool can handle.
     * The tool will only be activated if an object of one of these types
     * is seen in the probed application.
     * @return a QVector<QByteArray> of class names of types this tool supports.
     */
    const QVector<QByteArray> &supportedTypes() const;
    /*!
     * Set names of supported classes.
     * @see supportedTypes()
     * @since 2.5
     */
    void setSupportedTypes(const QVector<QByteArray> &types);

    /*!
     * Class names of types this tool can handle as a string.
     * @return a comma separated QString of class names of types this tool supports.
     */
    QString supportedTypesString() const;

    /*!
     * Initialize the tool.
     * Implement this method to do non-GUI initialization, such as creating
     * object tracking models etc.
     * @param probe The probe interface allowing access to the object models.
     */
    virtual void init(Probe *probe) = 0;

    /*!
     * Allows to hide a plug-in from the UI.
     * This is useful for plug-ins that only provide support for additional
     * data types. The value is usually filled in by the plug-in loader
     * @return @c true if the plug-in has no tool view.
     * @since 2.1
     */
    virtual bool isHidden() const;

    /*!
     * Class names of types this tool can select.
     * This must be a subset of supportedTypes(), and is used to check if this
     * tool is a viable candidate for object navigation.
     * When returning an non-empty result here, you must handle the Probe::objectSelected()
     * signal.
     */
    virtual QVector<QByteArray> selectableTypes() const;

private:
    Q_DISABLE_COPY(ToolFactory)
    QVector<QByteArray> m_types;
};

/**
 * @brief A templated generic ToolFactory for some data type and tool.
 */
template<typename Type, typename Tool>
class StandardToolFactory : public ToolFactory
{
public:
    StandardToolFactory()
    {
        setSupportedTypes(QVector<QByteArray>() << Type::staticMetaObject.className());
    }

    QString id() const override
    {
        return Tool::staticMetaObject.className();
    }

    void init(Probe *probe) override
    {
        new Tool(probe, probe);
    }
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ToolFactory, "com.kdab.GammaRay.ToolFactory/1.0")
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::ToolFactory *)

#endif
