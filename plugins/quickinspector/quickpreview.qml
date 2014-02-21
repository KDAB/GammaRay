import QtQuick 1.0

Rectangle {
  id: root
  color: "orange"

  Image {
    source: "image://quicksceneprovider/background"
    anchors.fill: parent
    fillMode: Image.Tile
  }

  function updatePreview()
  {
    image.source = "image://quicksceneprovider/scene.png";
    image.zoom = root.width / image.sourceSize.width;
    image.x = 0;
    image.y = (root.height - image.height) / 2;
  }

  Image {
    id: image
    property real zoom: 1

    onZoomChanged: {
      x += (width - sourceSize.width * zoom) / 2;
      y += (height - sourceSize.height * zoom) / 2;
      width = sourceSize.width * zoom;
      height = sourceSize.height * zoom
    }

    MouseArea {
      id: imageMA
      anchors.fill: parent
      property int oldMouseX
      property int oldMouseY

      onPressed: {
        oldMouseX = mouse.x;
        oldMouseY = mouse.y;
      }
      onPositionChanged: {
        var shiftX = mouse.x - oldMouseX;
        var shiftY = mouse.y - oldMouseY;
        image.x += shiftX;
        image.y += shiftY;
        oldMouseX = mouse.x - shiftX;
        oldMouseY = mouse.y - shiftY;
      }
    }
  }

  Text {
    anchors { top: parent.top; right: parent.right; margins: 10 }
    text: imageMA
  }

  Row {
    anchors { right: parent.right; bottom: parent.bottom; margins: 10 }
    spacing: 5

    Rectangle {
      color: "lightgrey"
      border.color: "grey"
      width: 20; height: width

      Text {
        text: "-"
        anchors.centerIn: parent
      }

      MouseArea {
        anchors.fill: parent
        onPressed: {
          zoomTimer.summand = -0.1;
          zoomTimer.start();
        }
        onReleased: zoomTimer.stop();
      }
    }
    Rectangle {
      color: "lightgrey"
      border.color: "grey"
      width: 20; height: width

      Text {
        text: "+"
        anchors.centerIn: parent
      }

      MouseArea {
        anchors.fill: parent
        onPressed: {
          zoomTimer.summand = 0.1;
          zoomTimer.start();
        }
        onReleased: zoomTimer.stop();
      }
    }
  }

  Timer {
    id: zoomTimer
    interval: 100
    repeat: true
    triggeredOnStart: true
    property real summand: 1
    onTriggered: image.zoom += summand;
  }
}