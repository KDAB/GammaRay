/*
  propertywidget.h

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

#ifndef GAMMARAY_PROPERTYWIDGET_H
#define GAMMARAY_PROPERTYWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QHash>

#include "gammaray_ui_export.h"
#include <common/enums.h>

class QAbstractItemModel;
class QAbstractItemView;
class QModelIndex;

namespace GammaRay {

class Ui_PropertyWidget;
class PropertyControllerInterface;

/** Client-side counter-part GammaRay::PropertyController. */
class GAMMARAY_UI_EXPORT PropertyWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit PropertyWidget(QWidget *parent = 0);
    virtual ~PropertyWidget();

    void setObjectBaseName(const QString &baseName);

  private slots:
    void setDisplayState(GammaRay::PropertyWidgetDisplayState::State state);
    void methodActivated(const QModelIndex &index);
    void methodConextMenu(const QPoint &pos);

    void onDoubleClick(const QModelIndex &index);

  private:
    /// Decides if widget is supposed to be shown at this display state
    bool showTab(const QWidget *widget, PropertyWidgetDisplayState::State state) const;

    QAbstractItemModel* model(const QString &nameSuffix);

    Ui_PropertyWidget *m_ui;

    QString m_objectBaseName;

    // Contains initially added tab widgets (Tab widget/Label)
    QVector< QPair<QWidget *,QString> > m_tabWidgets;

    PropertyWidgetDisplayState::State m_displayState;
    PropertyControllerInterface *m_controller;
};

}

#endif // GAMMARAY_PROPERTYWIDGET_H
