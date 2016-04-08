import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.5 as QQ2


Entity {
    id: root;

    property rect topLevelGeometry
    property real explosionFactor: 0
    property int modelIndex: -1

    property int level
    property var frontTextureImage
    property var backTextureImage
    property rect geometry

    readonly property real _scaleFactor : 10.0
    readonly property real _geomWidth: root.geometry.width / _scaleFactor
    readonly property real _geomHeight: root.geometry.height / _scaleFactor
    readonly property real _geomX: root.geometry.x / _scaleFactor
    readonly property real _geomY: root.geometry.y / _scaleFactor

    components: [
        CuboidMesh {
            xExtent: _geomWidth
            yExtent: _geomHeight
            zExtent: 1
        },

        WidgetMaterial {
            id: material
            frontTextureImage: root.frontTextureImage
            backTextureImage: root.backTextureImage
            //highlighted: objectPicker.containsMouse
        },

        Transform {
            id: transform
            translation: Qt.vector3d(
                             _geomWidth / 2.0 + _geomX - topLevelGeometry.width / 2.0 / _scaleFactor,
                             -_geomHeight / 2.0 - _geomY + topLevelGeometry.height / 2.0 / _scaleFactor,
                             root.level / (_scaleFactor * 2.0) + root.level * explosionFactor
                         )
        }

        /*
        ObjectPicker {
            id: objectPicker
            hoverEnabled: true
            onClicked: console.log("Click!")
        }
        */
    ]
}
