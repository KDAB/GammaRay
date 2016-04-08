/*
  main.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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


import QtQuick 2.5 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

Entity {
    id: root

    property real explosionFactor: 0

    QQ2.Behavior on explosionFactor {
        QQ2.NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    // Render from the mainCamera
    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
                clearColor: "black"
                window: _surface
            }

            pickingSettings.pickMethod: PickingSettings.TrianglePicking
        },

        InputSettings {
            eventSource: _eventSource
        }
    ]


    Camera {
        id: mainCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: _window.width / _window.height
        nearPlane: 0.1
        farPlane: 1000.0
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        position: Qt.vector3d(0.0, 0.0, 80.0)
    }

    CameraController {
        id: cameraController
        camera: mainCamera
    }

    QQ2.Connections {
        target: _window
        onWheel: {
            root.explosionFactor += delta / 100.0;
            if (root.explosionFactor < 0) {
                root.explosionFactor = 0.0;
            }
        }
    }

    NodeInstantiator {
        id: instantiator;
        model: _widgetModel
        asynchronous: true
        delegate: WidgetDelegate {
            id: widgetDelegate
            // HACK: get top-level window geometry so we can transform children center accordingly
            topLevelGeometry: objectAt(0).geometry
            geometry: model.geometry
            level: model.level
            frontTextureImage: model.frontTexture
            backTextureImage: model.backTexture
            explosionFactor: root.explosionFactor;
        }
    }
}
