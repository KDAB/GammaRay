/*
  quickinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTOR_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTOR_H

#include "quickinspectorinterface.h"

#include <common/remoteviewinterface.h>
#include <core/toolfactory.h>

#include <QQuickWindow>
#include <QImage>
#include <QMutex>
#include <memory>

QT_BEGIN_NAMESPACE
class QQuickShaderEffectSource;
class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;
class QSGNode;
// class QSGBasicGeometryNode;
// class QSGGeometryNode;
// class QSGClipNode;
// class QSGTransformNode;
// class QSGRootNode;
// class QSGOpacityNode;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;
class AbstractScreenGrabber;
class GrabbedFrame;
struct QuickDecorationsSettings;
class QuickItemModel;
class QuickSceneGraphModel;
class RemoteViewServer;
class ObjectId;
class PaintAnalyzer;
using ObjectIds = QVector<ObjectId>;

class RenderModeRequest : public QObject
{
    Q_OBJECT

public:
    explicit RenderModeRequest(QObject *parent = nullptr);
    ~RenderModeRequest() override;

    void applyOrDelay(QQuickWindow *toWindow, QuickInspectorInterface::RenderMode customRenderMode);

signals:
    void aboutToCleanSceneGraph();
    void sceneGraphCleanedUp();
    void finished();

private slots:
    void apply();
    void preFinished();

private:
    static QMutex mutex;

    QuickInspectorInterface::RenderMode mode;
    QMetaObject::Connection connection;
    QPointer<QQuickWindow> window;
};

class QuickInspector : public QuickInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::QuickInspectorInterface)

public:
    explicit QuickInspector(Probe *probe, QObject *parent = nullptr);
    ~QuickInspector() override;

signals:
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);

public slots:
    void selectWindow(int index) override;

    void setCustomRenderMode(GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
        override;

    void checkFeatures() override;

    void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings) override;

    void checkOverlaySettings() override;

    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void pickElementId(const GammaRay::ObjectId &id);

    void sendRenderedScene(const GammaRay::GrabbedFrame &grabbedFrame);

    void analyzePainting() override;

    void checkSlowMode() override;
    void setSlowMode(bool slow) override;

protected:
    bool eventFilter(QObject *receiver, QEvent *event) override;

private slots:
    void slotGrabWindow();
    void itemSelectionChanged(const QItemSelection &selection);
    void sgSelectionChanged(const QItemSelection &selection);
    void sgNodeDeleted(QSGNode *node);
    void qObjectSelected(QObject *object);
    void nonQObjectSelected(void *object, const QString &typeName);
    void objectCreated(QObject *object);
    void recreateOverlay();
    void aboutToCleanSceneGraph();
    void sceneGraphCleanedUp();

private:
    void selectWindow(QQuickWindow *window);
    void selectItem(QQuickItem *item);
    void selectSGNode(QSGNode *node);
    static void registerMetaTypes();
    static void registerVariantHandlers();
    static void registerPCExtensions();
    QString findSGNodeType(QSGNode *node) const;
    static void scanForProblems();

    GammaRay::ObjectIds recursiveItemsAt(QQuickItem *parent, const QPointF &pos,
                                         GammaRay::RemoteViewInterface::RequestMode mode,
                                         int &bestCandidate, bool parentIsGoodCandidate = true) const;

    Probe *m_probe;
    std::unique_ptr<AbstractScreenGrabber> m_overlay;
    QPointer<QQuickWindow> m_window;
    QPointer<QQuickItem> m_currentItem;
    QSGNode *m_currentSgNode;
    QAbstractItemModel *m_windowModel;
    QuickItemModel *m_itemModel;
    QItemSelectionModel *m_itemSelectionModel;
    QuickSceneGraphModel *m_sgModel;
    QItemSelectionModel *m_sgSelectionModel;
    PropertyController *m_itemPropertyController;
    PropertyController *m_sgPropertyController;
    RemoteViewServer *m_remoteView;
    RenderModeRequest *m_pendingRenderMode;
    QuickInspectorInterface::RenderMode m_renderMode;
    PaintAnalyzer *m_paintAnalyzer;
    bool m_slowDownEnabled;
};

class QuickInspectorFactory : public QObject,
                              public StandardToolFactory<QQuickWindow, QuickInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_quickinspector.json")

public:
    explicit QuickInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif
