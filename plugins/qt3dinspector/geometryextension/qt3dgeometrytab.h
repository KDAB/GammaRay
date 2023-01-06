/*
  qt3dgeometrytab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QT3DGEOMETRYTAB_H
#define GAMMARAY_QT3DGEOMETRYTAB_H

#include "boundingvolume.h"

#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Qt3DCore {
class QAspectEngine;
class QComponent;
class QNode;
class QTransform;
}
namespace Qt3DRender {
class QCamera;
class QCullFace;
class QDepthTest;
class QGeometryRenderer;
class QParameter;
class QPickEvent;
class QRenderPass;
}
class QComboBox;
class QSurfaceFormat;
QT_END_NAMESPACE

namespace GammaRay {
class BufferModel;
class PropertyWidget;
class Qt3DGeometryExtensionInterface;
struct Qt3DGeometryAttributeData;

namespace Ui {
class Qt3DGeometryTab;
}

class Qt3DGeometryTab : public QWidget
{
    Q_OBJECT
public:
    explicit Qt3DGeometryTab(PropertyWidget *parent);
    ~Qt3DGeometryTab();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *receiver, QEvent *event) override;

private:
    Qt3DCore::QComponent *createMaterial(Qt3DCore::QNode *parent);
    Qt3DCore::QComponent *createES2WireframeMaterial(Qt3DCore::QNode *parent);
    static Qt3DCore::QComponent *createSkyboxMaterial(Qt3DCore::QNode *parent);
    void updateGeometry();
    void resetCamera();
    void computeBoundingVolume(const Qt3DGeometryAttributeData &vertexAttr,
                               const QByteArray &bufferData);
    void trianglePicked(Qt3DRender::QPickEvent *pick);
    bool isIndexBuffer(unsigned int bufferIndex) const;
    QSurfaceFormat probeFormat() const;

    std::unique_ptr<Ui::Qt3DGeometryTab> ui;
    QComboBox *m_shadingModeCombo;
    Qt3DGeometryExtensionInterface *m_interface;

    QWindow *m_surface;
    Qt3DCore::QAspectEngine *m_aspectEngine;
    Qt3DRender::QCamera *m_camera;
    Qt3DRender::QGeometryRenderer *m_geometryRenderer;
    Qt3DRender::QGeometryRenderer *m_es2lineRenderer; // ES2 wireframe fallback renderer
    Qt3DCore::QTransform *m_geometryTransform;
    Qt3DRender::QCullFace *m_cullMode;
    Qt3DRender::QDepthTest *m_depthTest;
    Qt3DRender::QRenderPass *m_normalsRenderPass;
    Qt3DRender::QParameter *m_normalLength;
    Qt3DRender::QParameter *m_shadingMode;
    BoundingVolume m_boundingVolume;
    mutable bool m_usingES2Fallback = false;

    BufferModel *m_bufferModel;
};
}

#endif // GAMMARAY_QT3DGEOMETRYTAB_H
