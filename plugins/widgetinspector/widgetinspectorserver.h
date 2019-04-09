/*
  widgetinspectorserver.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <common/remoteviewinterface.h>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QItemSelection;
class QItemSelectionModel;
class QLibrary;
class QPoint;
QT_END_NAMESPACE

namespace GammaRay {
class Probe;
class PropertyController;
class OverlayWidget;
class PaintAnalyzer;
class RemoteViewServer;
class ObjectId;
using ObjectIds = QVector<ObjectId>;

class WidgetInspectorServer : public WidgetInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::WidgetInspectorInterface)
public:
    explicit WidgetInspectorServer(Probe *probe, QObject *parent = nullptr);
    ~WidgetInspectorServer() override;

signals:
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    GammaRay::ObjectIds recursiveWidgetsAt(QWidget *parent, const QPoint &pos,
                                           GammaRay::RemoteViewInterface::RequestMode mode, int& bestCandidate) const;
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);
    QImage imageForWidget(QWidget *widget);
    void registerWidgetMetaTypes();
    void registerVariantHandlers();
    void discoverObjects();
    void checkFeatures();
    QVector<QRect> tabFocusChain(QWidget *window) const;

private slots:
    void widgetSelectionChanged(const QItemSelection &selection);
    void widgetSelected(QWidget *widget);
    void objectSelected(QObject *obj);
    void objectCreated(QObject *object);

    void recreateOverlayWidget();

    void saveAsImage(const QString &fileName) override;
    void saveAsSvg(const QString &fileName) override;
    void saveAsPdf(const QString &fileName) override;
    void saveAsUiFile(const QString &fileName) override;

    void analyzePainting() override;

    void updateWidgetPreview();

    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void pickElementId(const GammaRay::ObjectId& id);

private:
    QPointer<OverlayWidget> m_overlayWidget;
    QLibrary *m_externalExportActions;
    PropertyController *m_propertyController;
    QItemSelectionModel *m_widgetSelectionModel;
    QPointer<QWidget> m_selectedWidget;
    PaintAnalyzer *m_paintAnalyzer;
    RemoteViewServer *m_remoteView;
    Probe *m_probe;
};
}

#endif // GAMMARAY_WIDGETINSPECTORSERVER_H
