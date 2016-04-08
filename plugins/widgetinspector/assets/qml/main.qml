import QtQuick 2.5 as QQ2
import QtQml 2.2 as QQml2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0

Entity {
    id: root

    property real explosionFactor: 0

    // Render from the mainCamera
    components: [
        FrameGraph {
            activeFrameGraph: ForwardRenderer {
                id: renderer
                camera: mainCamera
                clearColor: "black"
            }

            pickingSettings.pickMethod: PickMethod.TrianglePicking
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
