/*
  propertywidgettab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef PROPERTYWIDGETTAB_H
#define PROPERTYWIDGETTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>
#include <QString>

namespace GammaRay {
class PropertyWidget;

/*! Priority values for property widget extension tabs.
 *  It is recommended to specify tab priorities relating to these
 *  constants.
 */
namespace PropertyWidgetTabPriority {
/*! Tab priority constants. */
enum Priority {
    First = 0,     ///< The main property inspector, the left-most one.
    Basic = 100,     ///< QObject data that is of common use.
    Advanced = 200,     ///< Tools with common and high value use for a small sub-set of classes.
    Exotic = 300     ///< Rarely used information, or data that is only valuable to a small amount of users.
};
}

/*! Interface for tabs in the property widget. */
class GAMMARAY_UI_EXPORT PropertyWidgetTabFactoryBase
{
public:
    explicit PropertyWidgetTabFactoryBase(const QString &name, const QString &label, int priority);
    virtual ~PropertyWidgetTabFactoryBase();

    virtual QWidget *createWidget(PropertyWidget *parent) = 0;

    QString name() const;
    QString label() const;
    int priority() const;

private:
    Q_DISABLE_COPY(PropertyWidgetTabFactoryBase)

    QString m_name;
    QString m_label;
    int m_priority;
};

/*! Template implementation of PropertyWidgetTabFactoryBase. */
template<typename T>
class PropertyWidgetTabFactory : public PropertyWidgetTabFactoryBase
{
public:
    explicit PropertyWidgetTabFactory(const QString &name, const QString &label, int priority)
        : GammaRay::PropertyWidgetTabFactoryBase(name, label, priority) {}

    QWidget *createWidget(PropertyWidget *parent) override
    {
        return new T(parent);
    }
};
}

#endif // PROPERTYWIDGETTAB_H
