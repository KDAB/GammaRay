/*
  widgetinspector.h

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

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTOR_H

#include "widgetinspectorwidget.h"
#include "include/toolfactory.h"
#include <core/remote/objectserver.h>

#include <QLibrary>

class QItemSelection;
class QItemSelectionModel;
class QModelIndex;

namespace GammaRay {

class PropertyController;
class OverlayWidget;

class WidgetInspector : public ObjectServer
{
  Q_OBJECT
  public:
    explicit WidgetInspector(ProbeInterface *probe, QObject *parent = 0);

    void selectDefaultItem();

  private:
    QWidget *selectedWidget() const;
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);

  private slots:
    void widgetSelected(const QItemSelection &selection);
    void widgetSelected(QWidget *widget);

    void handleOverlayWidgetDestroyed(QObject*);

    void saveAsImage(const QString &fileName);
    void saveAsSvg(const QString &fileName);
    void saveAsPdf(const QString &fileName);
    void saveAsUiFile(const QString &fileName);

    void analyzePainting();

  private:
    OverlayWidget *m_overlayWidget;
    QLibrary m_externalExportActions;
    PropertyController *m_propertyController;
    QItemSelectionModel *m_widgetSelectionModel;
};

class WidgetInspectorFactory
  : public QObject, public StandardToolFactory2<QWidget, WidgetInspector, WidgetInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.WidgetInspector")
  public:
    explicit WidgetInspectorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Widgets");
    }
};

}

#endif // GAMMARAY_WIDGETINSPECTOR_H
