import QtLocation 5.3
import QtQuick 2.0

Item {
    Plugin {
        id: mapPlugin
        required.mapping: Plugin.AnyMappingFeatures
        preferred: [ "osm" ]
    }

    Map {
        anchors.fill: parent
        plugin: mapPlugin
    }
}
