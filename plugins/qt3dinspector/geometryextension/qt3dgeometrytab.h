/*
  qt3dgeometrytab.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QT3DGEOMETRYTAB_H
#define GAMMARAY_QT3DGEOMETRYTAB_H

#include <QWidget>

#include <memory>

namespace Qt3DCore {
class QAspectEngine;
class QComponent;
class QNode;
}
namespace Qt3DRender {
class QCamera;
class QGeometryRenderer;
class QRenderPass;
}

namespace GammaRay {

class PropertyWidget;
class Qt3DGeometryExtensionInterface;

namespace Ui
{
class Qt3DGeometryTab;
}

class Qt3DGeometryTab : public QWidget
{
    Q_OBJECT
public:
    explicit Qt3DGeometryTab(PropertyWidget *parent);
    ~Qt3DGeometryTab();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Qt3DCore::QComponent* createMaterial(Qt3DCore::QNode *parent);
    void updateGeometry();
    void resetCamera();

    std::unique_ptr<Ui::Qt3DGeometryTab> ui;
    Qt3DGeometryExtensionInterface *m_interface;

    QWindow* m_surface;
    Qt3DCore::QAspectEngine *m_aspectEngine;
    Qt3DRender::QCamera *m_camera;
    Qt3DRender::QGeometryRenderer *m_geometryRenderer;
    Qt3DRender::QRenderPass *m_normalsRenderPass;
};
}

#endif // GAMMARAY_QT3DGEOMETRYTAB_H
