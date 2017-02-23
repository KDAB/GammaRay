import QtQuick 2.0
import QtQuick.Controls 1.0

Item {
    id: a
    width: 200
    height: 200
    property int contextPropertyFoo: 5

    Item {
        id: b
        x: 5
        y: 6
        width: x + contextPropertyFoo
        height: x < 10 ? width : y

        Component.onCompleted: {
            b.visible = Qt.binding(function(){ return x < 15; });
        }
    }

    Item {
        id: c
        property bool wantBindingLoop: false
        x: b.x
        y: height
        width: x + y
        height: wantBindingLoop ? width : x
    }

    Column {
        anchors.centerIn: parent
        Button {
            text: "b.x++"
            onClicked: b.x++;
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "b.y++"
            onClicked: b.x++;
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "Toggle binding loop"
            onClicked: c.wantBindingLoop = !c.wantBindingLoop;
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
