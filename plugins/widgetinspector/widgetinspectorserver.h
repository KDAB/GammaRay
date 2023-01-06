/*
  widgetinspectorserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORSERVER_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORSERVER_H

#include <widgetinspectorinterface.h>
#include <common/remoteviewinterface.h>

#include <QPointer>

#include <memory>

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
                                           GammaRay::RemoteViewInterface::RequestMode mode, int &bestCandidate) const;
    void callExternalExportAction(const char *name, QWidget *widget, const QString &fileName);
    QImage imageForWidget(QWidget *widget);
    static void registerWidgetMetaTypes();
    static void registerVariantHandlers();
    void discoverObjects();
    void checkFeatures();
    static QVector<QRect> tabFocusChain(QWidget *window);

private slots:
    void widgetSelectionChanged(const QItemSelection &selection);
    void widgetSelected(QWidget *widget);
    void objectSelected(QObject *obj);
    void objectCreated(QObject *object);

    void recreateOverlayWidget();

    void saveAsImage(const QString &fileName) override;
    void saveAsSvg(const QString &fileName) override;
    void saveAsUiFile(const QString &fileName) override;

    void analyzePainting() override;

    void updateWidgetPreview();

    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void pickElementId(const GammaRay::ObjectId &id);

private:
    QPointer<OverlayWidget> m_overlayWidget;
    std::unique_ptr<QLibrary> m_externalExportActions;
    PropertyController *m_propertyController;
    QItemSelectionModel *m_widgetSelectionModel;
    QPointer<QWidget> m_selectedWidget;
    PaintAnalyzer *m_paintAnalyzer;
    RemoteViewServer *m_remoteView;
    Probe *m_probe;
};
}

#endif // GAMMARAY_WIDGETINSPECTORSERVER_H
