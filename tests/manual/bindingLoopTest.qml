import QtQuick 2.0

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
    }

    Item {
        id: c
        x: b.x
        y: height
        width: x + y
        height: width
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            print("Hallo");
            if (mouse.modifiers & Qt.ControlModifier) {
                print("y");
                b.y++;
            } else {
                print("x");
                b.x++;
            }
        }
    }
}
