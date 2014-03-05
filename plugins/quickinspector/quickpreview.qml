import QtQuick 1.1
import com.kdab.GammaRay 1.0

Image {
  id: root
  source: "image://quicksceneprovider/background"
  fillMode: Image.Tile
  property real oldWidth
  property real oldHeight
  property variant geometryData

  focus: true

  Keys.onPressed: {
    inspectorInterface.sendKeyEvent(6, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
  }
  Keys.onReleased: {
    inspectorInterface.sendKeyEvent(7, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
  }

  function updatePreview()
  {
    image.source = "";
    image.source = "image://quicksceneprovider/scene";
    image.x = (root.width - image.width - rightRuler.width) / 2;
    image.y = (root.height - image.height - bottomRuler.height) / 2;
    oldWidth = root.width;
    oldHeight = root.height;
  }

  onWidthChanged: {
    image.x += (width - oldWidth) / 2;
    oldWidth = width;
  }
  onHeightChanged: {
    image.y += (height - oldHeight) / 2;
    oldHeight = height;
  }

  Image {
    id: image
    property real zoom: 1
    cache: false

    onZoomChanged: {
      x += (width - sourceSize.width * zoom) / 2;
      y += (height - sourceSize.height * zoom) / 2;
      width = sourceSize.width * zoom;
      height = sourceSize.height * zoom
    }
  }

  QuickItemOverlay {
    anchors.fill: parent
    zoom: image.zoom
    geometryData: root.geometryData
    imageRect: Qt.rect(image.x, image.y, image.width, image.height);
  }

  MouseArea {
    id: imageMA
    anchors.fill: parent
    hoverEnabled: true
    property int oldMouseX
    property int oldMouseY
    property int modifiers: 0


    onReleased: {
      if (mouse.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(3, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
    }
    onPressed: {
      if (mouse.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(2, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
      oldMouseX = mouse.x;
      oldMouseY = mouse.y;
    }
    onPositionChanged: {
      if (mouse.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(5, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
      else if (pressed) {
        modifiers = mouse.modifiers
        if (modifiers !== Qt.ControlModifier) {
          image.x += mouse.x - oldMouseX;
          image.y += mouse.y - oldMouseY;
          oldMouseX = mouse.x;
          oldMouseY = mouse.y;
        }
      }
    }
    onDoubleClicked: {
      if (mouse.modifiers & (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(4, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
    }
  }

  Rectangle {
    color: "#aa333333"
    width: overlayText.width; height: overlayText.height
    anchors { top: parent.top; right: rightRuler.left; margins: 5 }
    radius: 3

    Text {
      id: overlayText
      color: "lightgrey"
      text: imageMA.pressed && imageMA.modifiers == Qt.ControlModifier
            ? Math.floor((imageMA.oldMouseX - image.x) / image.zoom) + ", " + Math.floor((imageMA.oldMouseY - image.y) / image.zoom) + " - "
              + Math.floor((imageMA.mouseX - image.x) / image.zoom) + ", " + Math.floor((imageMA.mouseY - image.y) / image.zoom) + " -> "
              + Math.floor(Math.sqrt( Math.pow(imageMA.mouseX - imageMA.oldMouseX, 2) + Math.pow(imageMA.mouseY - imageMA.oldMouseY, 2) ) / image.zoom) + "px"
            : Math.floor((imageMA.mouseX - image.x) / image.zoom) + "x" + Math.floor((imageMA.mouseY - image.y) / image.zoom)
    }
  }

  Rectangle {
    id: bottomRuler
    height: 25
    width: parent.width
    color: "#aa333333"
    anchors.bottom: parent.bottom

    Item {
      width: parent.width - rightRuler.width
      height: parent.height
      clip: true

      Row {
        x: image.x
        spacing: image.zoom > 1 ? image.zoom - 1 : 1
        Repeater {
          model: image.sourceSize.width
          delegate: Rectangle {
            color: "#aaffffff"
            width: 1
            height: index % 10 == 0 ? 10 : 5
            visible: pixelNumber <= image.sourceSize.width

            property int pixelNumber: image.zoom > 1 ? index : index * 2 / image.zoom

            Text {
              color: "#aaffffff"
              anchors.horizontalCenter: parent.horizontalCenter
              anchors.top: parent.bottom
              visible: index % (image.zoom <= 2 ? 20 : 10) == 0
              text: pixelNumber
            }
          }
        }
      }
    }
  }
  Rectangle {
    id: rightRuler
    width: 40
    height: parent.height
    color: "#aa333333"
    anchors.right: parent.right

    Item {
      width: parent.width
      height: parent.height - bottomRuler.height
      clip: true

      Column {
        y: image.y
        spacing: image.zoom > 1 ? image.zoom - 1 : 1
        Repeater {
          model: image.sourceSize.height
          delegate: Rectangle {
            color: "#aaffffff"
            height: 1
            width: index % 10 == 0 ? 10 : 5
            visible: pixelNumber <= image.sourceSize.height

            property int pixelNumber: image.zoom > 1 ? index : index * 2 / image.zoom

            Text {
              color: "#aaffffff"
              anchors.verticalCenter: parent.verticalCenter
              anchors.left: parent.right
              visible: index % 10 == 0
              text: pixelNumber
            }
          }
        }
      }
    }
  }

  Row {
    anchors { right: parent.right; top: bottomRuler.top }

    Rectangle {
      color: decrementZoomMA.containsMouse ? "#22ffffff" : "transparent"
      border.color: "grey"
      width: 20; height: width

      Text {
        text: "-"
        color: "grey"
        anchors.centerIn: parent
      }

      MouseArea {
        id: decrementZoomMA
        anchors.fill: parent
        hoverEnabled: true
        onPressed: decrementZoomTimer.start();
        onReleased: decrementZoomTimer.stop();
      }
    }
    Rectangle {
      color: incrementZoomMA.containsMouse ? "#22ffffff" : "transparent"
      border.color: "grey"
      width: 20; height: width

      Text {
        text: "+"
        color: "grey"
        anchors.centerIn: parent
      }

      MouseArea {
        id: incrementZoomMA
        anchors.fill: parent
        hoverEnabled: true
        onPressed: incrementZoomTimer.start();
        onReleased: incrementZoomTimer.stop();
      }
    }
  }

  Timer {
    id: incrementZoomTimer
    interval: 100
    repeat: true
    triggeredOnStart: true
    onTriggered: image.zoom = image.zoom < 1 ? 1 / (1 / image.zoom - 1) : image.zoom + 1;
  }
  Timer {
    id: decrementZoomTimer
    interval: 100
    repeat: true
    triggeredOnStart: true
    onTriggered: image.zoom = image.zoom <= 1 ? 1 / (1 / image.zoom + 1) : image.zoom - 1;
  }
}