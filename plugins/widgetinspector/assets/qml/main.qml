/*
  main.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
import QtQml 2.2 as Qml2
import QtQuick.Controls 1.2 as QQControls

import QtQuick.Scene3D 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import com.kdab.GammaRay 1.0

QQ2.Item {

    function resetView()
    {
        mainCamera.resetView()
        root.explosionFactor = 0
    }

    Scene3D {
        id: scene3d
        anchors.fill: parent
        focus: true
        hoverEnabled: true
        aspects: [ "input", "logic", "render" ]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
        multisample: true

        Entity {
            id: root

            property real explosionFactor: 0


            Camera {
                id: mainCamera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                nearPlane: 0.1
                farPlane: 1000.0
                viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
                upVector: Qt.vector3d(0.0, 1.0, 0.0)
                position: Qt.vector3d(0.0, 0.0, 80.0)

                function resetView()
                {
                    viewCenter = Qt.vector3d(0.0, 0.0, 0.0);
                    upVector = Qt.vector3d(0.0, 1.0, 0.0);
                    position = Qt.vector3d(0.0, 0.0, 80.0);
                }
            }

            // Render from the mainCamera
            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        camera: mainCamera
                        clearColor: "black"
                    }

                    pickingSettings.pickMethod: PickingSettings.TrianglePicking
                    // TODO: Does not work with Scene3D
                    //renderPolicy: RenderSettings.OnDemand
                    renderPolicy: RenderSettings.Always
                },

                InputSettings {
                    eventSource: _renderWindow
                }
            ]

            CameraController {
               id: cameraController
               camera: mainCamera
            }

            QQ2.Connections {
                target: _renderWindow
                onWheel: {
                    var newFactor = root.explosionFactor + delta / 80.0;
                    root.explosionFactor = newFactor < 0 ? 0 : newFactor;
                }
            }

            NodeInstantiator {
                id: instantiator
                model: _widgetModel
                asynchronous: false

                property var selectedWidget

                property var topLevelWindow : null

                delegate: WidgetDelegate {
                    id: windowDelegate

                    topLevelGeometry: instantiator.topLevelWindow !== null ? instantiator.topLevelWindow.geometry : model.geometry
                    objectId: model.objectId
                    geometry: model.geometry
                    frontTextureImage: model.frontTexture
                    backTextureImage: model.backTexture
                    explosionFactor: root.explosionFactor
                    depth: model.depth
                    metaData: model.metaData
                    isWindow: model.isWindow
                    wireframe: renderWireframe.checked
                    horizontals: renderHorizontals.checked

                    onSelectedChanged: if (selected) instantiator.selectedWidget = this
                }

                onObjectAdded: {
                    if (object.isWindow) {
                        topLevelWindow = object;
                    }
                }

                onObjectRemoved: {
                    if (topLevelWindow == object) {
                        topLevelWindow = null;
                    }
                }
            }

            Qml2.Binding {
                target: _selectionHelper
                property: "currentObject"
                value: instantiator.selectedWidget ? instantiator.selectedWidget.objectId : ""
            }
        }
    }

    WidgetInfo {
        id: widgetInfo;

        metaData: instantiator.selectedWidget && instantiator.selectedWidget.selected ?
                        instantiator.selectedWidget.metaData :
                        null

        anchors {
            top: parent.top
            right: parent.right
            margins: 20
        }
    }

    QQ2.Column {
        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: 20
        }

        CheckBox {
            id: renderWireframe
            text: qsTranslate("GammaRay::WidgetInspector::QML", "Wireframe")
        }

        CheckBox {
            id: renderHorizontals
            text: qsTranslate("GammaRay::WidgetInspector::QML", "Horizontals")
        }
    }
}
