/*
  propertywidgettab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QWidget>
#include <QString>

namespace GammaRay {

class PropertyWidget;

/** @brief Interface for tabs in the property widget. */
class PropertyWidgetTabFactoryBase
{
  public:
    explicit PropertyWidgetTabFactoryBase() {}
    virtual QWidget *createWidget(PropertyWidget *parent) = 0;
    virtual const QString &name() const = 0;
    virtual const QString &label() const = 0;
};

/** @brief Template implementation of PropertyWidgetTabFactoryBase. */
template <typename T>
class PropertyWidgetTabFactory : public PropertyWidgetTabFactoryBase
{
  public:
    explicit PropertyWidgetTabFactory(const QString &name, const QString &label)
      : m_name(name),
      m_label(label)
    {
    }
    QWidget *createWidget(PropertyWidget *parent)
    {
      return new T(parent);
    }
    const QString &name() const
    {
      return m_name;
    }
    const QString &label() const
    {
      return m_label;
    }
  private:
    QString m_name;
    QString m_label;
};

}

#endif // PROPERTYWIDGETTAB_H
