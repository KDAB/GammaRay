import QtQuick 2.0

Rectangle {
    id: root
    objectName: "AnchorsPropertyFilterTest"
    color: "lightsteelblue"
    width: 200; height: 100

    Rectangle {
        id: left
        objectName: "rectWithoutAnchors"
        width: 50; height: 50
        x: 25; y: 25
        color: "red"
    }

    Rectangle {
        id: right
        objectName: "rectWithAnchors"
        width: 50; height: 50
       anchors.verticalCenter: parent.verticalCenter
       anchors.right: parent.right
       anchors.rightMargin: 25
//         x: 125; y: 25
        color: "yellow"
    }

}
