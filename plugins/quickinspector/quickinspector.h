/*
  qmlsupport.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTOR_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTOR_H

#include "quickinspectorinterface.h"

#include <common/remoteviewinterface.h>
#include <core/toolfactory.h>

#include <QQuickWindow>
#include <QImage>
#include <QMutex>

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
class QuickOverlay;
class GrabedFrame;
struct QuickDecorationsSettings;
class QuickItemModel;
class QuickSceneGraphModel;
class RemoteViewServer;
class ObjectId;
typedef QVector<ObjectId> ObjectIds;

class QuickInspector : public QuickInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::QuickInspectorInterface)

public:
    explicit QuickInspector(ProbeInterface *probe, QObject *parent = nullptr);
    ~QuickInspector();

signals:
    void elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate);

public slots:
    void selectWindow(int index) override;

    void setCustomRenderMode(GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
    override;

    void checkFeatures() override;

    void setServerSideDecorationsEnabled(bool enabled) override;

    void checkServerSideDecorations() override;

    void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings) override;

    void checkOverlaySettings() override;

    void requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode);
    void pickElementId(const GammaRay::ObjectId& id);

    void sendRenderedScene(const GammaRay::GrabedFrame &grabedFrame);

protected:
    bool eventFilter(QObject *receiver, QEvent *event) override;

private slots:
    void slotGrabWindow();
    void itemSelectionChanged(const QItemSelection &selection);
    void sgSelectionChanged(const QItemSelection &selection);
    void sgNodeDeleted(QSGNode *node);
    void objectSelected(QObject *object);
    void objectSelected(void *object, const QString &typeName);
    void objectCreated(QObject *object);
    void recreateOverlay();

private:
    void selectWindow(QQuickWindow *window);
    void selectItem(QQuickItem *item);
    void selectSGNode(QSGNode *node);
    void registerMetaTypes();
    void registerVariantHandlers();
    void registerPCExtensions();
    QString findSGNodeType(QSGNode *node) const;
    void applyRenderMode();

    GammaRay::ObjectIds recursiveItemsAt(QQuickItem *parent, const QPointF &pos,
                                         GammaRay::RemoteViewInterface::RequestMode mode, int& bestCandidate) const;

    ProbeInterface *m_probe;
    QPointer<QuickOverlay> m_overlay;
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
    struct {
        RenderMode mode;
        QMetaObject::Connection connection;
        QQuickWindow *window;
        QMutex mutex;
    } m_pendingRenderMode;
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
