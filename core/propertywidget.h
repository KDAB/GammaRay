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

#include "include/gammaray_export.h"

class QAbstractItemView;
class QModelIndex;
class QStandardItemModel;

namespace GammaRay {

class Ui_PropertyWidget;
class ConnectionFilterProxyModel;
class MultiSignalMapper;
class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class ObjectClassInfoModel;
class ObjectMethodModel;
class ObjectEnumModel;
class MetaPropertyModel;
class PropertyEditorFactory;

class GAMMARAY_EXPORT PropertyWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit PropertyWidget(QWidget *parent = 0);
    virtual ~PropertyWidget();

    void setObject(QObject *object);
    void setObject(void *object, const QString &className);
    void setMetaObject(const QMetaObject *metaObject);

  private:
    enum DisplayState {
      QObjectState, // full QObject instance
      ObjectState, // non-QObject instance
      MetaObjectState // QMetaObject instance only
    };
    void setDisplayState(DisplayState state);
    void setEditorFactory(QAbstractItemView *view);

  private slots:
    void methodActivated(const QModelIndex &index);
    void signalEmitted(QObject *sender, int signalIndex);
    void methodConextMenu(const QPoint &pos);

  private:
    /// Decides if widget is supposed to be shown at this display state
    bool showTab(const QWidget *widget, DisplayState state) const;

    Ui_PropertyWidget *m_ui;

    QPointer<QObject> m_object;
    ObjectStaticPropertyModel *m_staticPropertyModel;
    ObjectDynamicPropertyModel *m_dynamicPropertyModel;
    ObjectClassInfoModel *m_classInfoModel;
    ObjectMethodModel *m_methodModel;
    ConnectionFilterProxyModel *m_inboundConnectionModel;
    ConnectionFilterProxyModel *m_outboundConnectionModel;
    ObjectEnumModel *m_enumModel;
    MultiSignalMapper *m_signalMapper;
    QStandardItemModel *m_methodLogModel;
    MetaPropertyModel *m_metaPropertyModel;
    QScopedPointer<PropertyEditorFactory> m_editorFactory;

    // Contains initially added tab widgets (Tab widget/Label)
    QVector< QPair<QWidget *,QString> > m_tabWidgets;
};

}

#endif // GAMMARAY_PROPERTYWIDGET_H
