/*
  widgetinspectorserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORSERVER_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORSERVER_H

#include <widgetinspectorinterface.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QItemSelection;
class QItemSelectionModel;
class QLibrary;
class QPoint;
QT_END_NAMESPACE

namespace GammaRay {

class ProbeInterface;
class PropertyController;
class OverlayWidget;
class PaintAnalyzer;
class RemoteViewServer;

class WidgetInspectorServer : public WidgetInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::WidgetInspectorInterface)
  public:
    explicit WidgetInspectorServer(ProbeInterface *probe, QObject *parent = 0);
    ~WidgetInspectorServer();

  protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

  private:
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);
    QImage imageForWidget(QWidget *widget);
    void registerWidgetMetaTypes();
    void registerVariantHandlers();
    void discoverObjects();
    void checkFeatures();

  private slots:
    void widgetSelected(const QItemSelection &selection);
    void widgetSelected(QWidget *widget);
    void objectSelected(QObject *obj);
    void objectCreated(QObject *object);

    void recreateOverlayWidget();

    void saveAsImage(const QString &fileName) Q_DECL_OVERRIDE;
    void saveAsSvg(const QString &fileName) Q_DECL_OVERRIDE;
    void saveAsPdf(const QString &fileName) Q_DECL_OVERRIDE;
    void saveAsUiFile(const QString &fileName) Q_DECL_OVERRIDE;

    void analyzePainting() Q_DECL_OVERRIDE;

    void updateWidgetPreview();
    void pickElement(const QPoint &pos);

  private:
    QPointer<OverlayWidget> m_overlayWidget;
    QLibrary *m_externalExportActions;
    PropertyController *m_propertyController;
    QItemSelectionModel *m_widgetSelectionModel;
    QPointer<QWidget> m_selectedWidget;
    PaintAnalyzer *m_paintAnalyzer;
    RemoteViewServer *m_remoteView;
    ProbeInterface *m_probe;
};

}

#endif // GAMMARAY_WIDGETINSPECTORSERVER_H
