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

    enabled: modelIndex != -1

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
            level: root.level
            frontTextureImage: root.frontTextureImage
            backTextureImage: root.backTextureImage
            explosionFactor: root.explosionFactor
            //highlighted: objectPicker.containsMouse

            QQ2.Component.onCompleted: console.log("WidgetDelegate for index " + root.modelIndex + " created");
            QQ2.Component.onDestruction: console.log("WidgetDelegate for index " + root.modelIndex + " destroyed");

        },

        Transform {
            id: transform
            translation: Qt.vector3d(
                             _geomWidth / 2.0 + _geomX - topLevelGeometry.width / 2.0 / _scaleFactor,
                             -_geomHeight / 2.0 - _geomY + topLevelGeometry.height / 2.0 / _scaleFactor,
                             root.level / (_scaleFactor * 2.0) + root.level * explosionFactor
                         )
        }

        /* FIXME: ObjectPicker on Cuboids is broken, reporting events even
           when there's no hit
        ObjectPicker {
            id: objectPicker
            hoverEnabled: true
            onClicked: console.log(widget.qWidget.objectName + ": Clicked!");
        }
        */
    ]
}
