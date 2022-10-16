/*
  propertywidgettab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
enum Priority
{
    First = 0, ///< The main property inspector, the left-most one.
    Basic = 100, ///< QObject data that is of common use.
    Advanced = 200, ///< Tools with common and high value use for a small sub-set of classes.
    Exotic = 300 ///< Rarely used information, or data that is only valuable to a small amount of users.
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
        : GammaRay::PropertyWidgetTabFactoryBase(name, label, priority)
    {
    }

    QWidget *createWidget(PropertyWidget *parent) override
    {
        return new T(parent);
    }
};
}

#endif // PROPERTYWIDGETTAB_H
