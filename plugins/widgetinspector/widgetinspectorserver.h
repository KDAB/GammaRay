/*
  widgetinspectorserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_WIDGETINSPECTORSERVER_H
#define GAMMARAY_WIDGETINSPECTORSERVER_H

#include <widgetinspectorinterface.h>

#include <QLibrary>
#include <QPointer>

class QModelIndex;
class QItemSelection;
class QItemSelectionModel;
class QTimer;

namespace GammaRay {

class ProbeInterface;
class PropertyController;
class OverlayWidget;
class PaintBufferModel;

class WidgetInspectorServer : public WidgetInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::WidgetInspectorInterface)
  public:
    explicit WidgetInspectorServer(ProbeInterface *probe, QObject *parent = 0);
    ~WidgetInspectorServer();

    void selectDefaultItem();

  protected:
    bool eventFilter(QObject *object, QEvent *event);

  private:
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);
    QPixmap pixmapForWidget(QWidget *widget);
    void registerWidgetMetaTypes();
    void registerVariantHandlers();
    void discoverObjects();

  private slots:
    void widgetSelected(const QItemSelection &selection);
    void widgetSelected(QWidget *widget);
    void objectCreated(QObject *object);

    void recreateOverlayWidget();

    void saveAsImage(const QString &fileName);
    void saveAsSvg(const QString &fileName);
    void saveAsPdf(const QString &fileName);
    void saveAsUiFile(const QString &fileName);

    void analyzePainting();
    void eventuallyUpdatePaintAnalyzer();
    void updatePaintAnalyzer();

    void updateWidgetPreview();

    void checkFeatures();

  private:
    QPointer<OverlayWidget> m_overlayWidget;
    QLibrary m_externalExportActions;
    PropertyController *m_propertyController;
    QItemSelectionModel *m_widgetSelectionModel;
    QPointer<QWidget> m_selectedWidget;
    QTimer *m_updatePreviewTimer;
    PaintBufferModel *m_paintBufferModel;
    QTimer *m_paintAnalyzerTimer;
    ProbeInterface *m_probe;
};

}

#endif // GAMMARAY_WIDGETINSPECTORSERVER_H
