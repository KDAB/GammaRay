/*
  propertywidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROPERTYWIDGET_H
#define GAMMARAY_PROPERTYWIDGET_H

#include <QTabWidget>
#include <QPointer>
#include <QHash>

#include "gammaray_ui_export.h"
#include <common/enums.h>
#include "propertywidgettab.h"

class QAbstractItemModel;
class QAbstractItemView;
class QModelIndex;

namespace GammaRay {

class Ui_PropertyWidget;
class PropertyControllerInterface;

/** @brief Client-side counter-part GammaRay::PropertyController. */
class GAMMARAY_UI_EXPORT PropertyWidget : public QTabWidget
{
  Q_OBJECT
  public:
    explicit PropertyWidget(QWidget *parent = 0);
    virtual ~PropertyWidget();

    QString objectBaseName() const;
    void setObjectBaseName(const QString &baseName);

    template<typename T> static void registerTab(const QString &name, const QString &label)
    {
      s_tabFactories << new PropertyWidgetTabFactory<T>(name, label);
      foreach (PropertyWidget *widget, s_propertyWidgets)
        widget->createWidgets();
    }

  private:
    void createWidgets();
    bool extensionAvailable(PropertyWidgetTabFactoryBase *factory) const;

  private slots:
    void updateShownTabs();


  private:
    QString m_objectBaseName;

    // Contains all tab widgets we have instantiated and their corresponding factories
    // order matters, therefore these are two vectors rather than a hash or map
    QVector<PropertyWidgetTabFactoryBase*> m_usedFactories;
    QVector<QWidget*> m_tabWidgets;

    PropertyControllerInterface *m_controller;

    static QVector<PropertyWidgetTabFactoryBase*> s_tabFactories;
    static QVector<PropertyWidget*> s_propertyWidgets;
};

}

#endif // GAMMARAY_PROPERTYWIDGET_H
