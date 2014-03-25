import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import com.kdab.GammaRay 1.0

Image {
  id: root
  source: "image://quicksceneprovider/background"
  fillMode: Image.Tile
  property real oldWidth: 0
  property real oldHeight: 0
  property variant geometryData: {}
  property bool isFirstFrame: true
  property bool canVisualizeOverdraw: true

  focus: true

  Keys.onPressed: { // event-forwarding
    inspectorInterface.sendKeyEvent(6, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
  }
  Keys.onReleased: { // event-forwarding
    inspectorInterface.sendKeyEvent(7, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
  }

  function updatePreview()
  {
    image.source = ""; // needed in order to get quicksceneprovider/scene to get refreshed.
    image.source = "image://quicksceneprovider/scene";
    image.width = image.sourceSize.width * image.zoom;
    image.height = image.sourceSize.height * image.zoom;

    // Align image to center
    if (isFirstFrame) {
      image.x = (root.width - image.width - rightRuler.width) / 2;
      image.y = (root.height - image.height - bottomRuler.height) / 2;
      isFirstFrame = false;
    }
  }

  onWidthChanged: {
    // Make scene preview stay centered when resizing
    image.x += (width - oldWidth) / 2;
    oldWidth = width;
  }
  onHeightChanged: {
    // Make scene preview stay centered when resizing
    image.y += (height - oldHeight) / 2;
    oldHeight = height;
  }

  Component {
    id: buttonStyle

    ButtonStyle {
      id: styleEl
      background: Rectangle {
        color: styleEl.control.hovered ? "#22ffffff" : "transparent"
        border.color: "grey"
      }
      label: Text {
        color: "grey"
        text: styleEl.control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }
    }
  }

  // Toolbar (top-left)
  Rectangle {
    color: "#aa333333"
    width: toolbarRow.width + 7; height: toolbarRow.height + 1
    x: 5; y: 5; z: 1
    radius: 4
    visible: toolbarRow.width

    Row {
      id: toolbarRow
      x: 3

      Button {
        height: 20
        visible: canVisualizeOverdraw
        property bool checked: false

        text: "Show 3D Box Model"
        style: buttonStyle

        onClicked: {
          checked = !checked;
          inspectorInterface.setVisualizeOverdraw(checked);
          geometryOverlay.visible = !checked;
        }
      }
    }
  }

  // Text item (top-right)
  Rectangle {
    color: "#aa333333"
    width: overlayText.width; height: overlayText.height
    anchors { top: parent.top; right: rightRuler.left; margins: 5 }
    radius: 3
    z: 1

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

  // Scene preview
  Image {
    id: image
    cache: false
    smooth: false
    property real zoom: 1

    function zoomIn(zoomToX, zoomToY) {
      var oldZoom = zoom;
      zoom = zoom < 1
            ? 1 / (1 / zoom - 1)
            : zoom + 1;
      x += zoomToX * (1 - zoom / oldZoom);
      y += zoomToY * (1 - zoom / oldZoom);
      width = sourceSize.width * zoom;
      height = sourceSize.height * zoom;
    }
    function zoomOut(zoomToX, zoomToY) {
      var oldZoom = zoom;
      zoom = zoom <= 1
            ? 1 / (1 / zoom + 1)
            : zoom - 1;
      x += zoomToX * (1 - zoom / oldZoom);
      y += zoomToY * (1 - zoom / oldZoom);
      width = sourceSize.width * zoom;
      height = sourceSize.height * zoom;
    }
  }

  // Geometry overlay
  QuickItemOverlay {
    id: geometryOverlay
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


    onReleased: { // event-forwarding
      if (mouse.modifiers == (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(3, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
    }
    onPressed: { // event-forwarding
      if (mouse.modifiers == (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(2, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
      oldMouseX = mouse.x;
      oldMouseY = mouse.y;
    }
    onPositionChanged: { // move image / event-forwarding
      if (mouse.modifiers == (Qt.ControlModifier | Qt.ShiftModifier)) // event-forwarding
        inspectorInterface.sendMouseEvent(5, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
      else if (pressed) { // move image
        modifiers = mouse.modifiers
        if (modifiers !== Qt.ControlModifier) {
          image.x += mouse.x - oldMouseX;
          image.y += mouse.y - oldMouseY;
          oldMouseX = mouse.x;
          oldMouseY = mouse.y;
        }
      }
    }
    onDoubleClicked: { // event-forwarding
      if (mouse.modifiers == (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendMouseEvent(4, Qt.point((mouse.x - image.x) / image.zoom, (mouse.y - image.y) / image.zoom), mouse.button, mouse.buttons, mouse.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
    }
    onWheel: { // event-forwarding
      if (wheel.modifiers == (Qt.ControlModifier | Qt.ShiftModifier))
        inspectorInterface.sendWheelEvent(Qt.point((wheel.x - image.x) / image.zoom, (wheel.y - image.y) / image.zoom), wheel.pixelDelta, wheel.angleDelta, wheel.buttons, wheel.modifiers & ~(Qt.ControlModifier | Qt.ShiftModifier));
      else if (wheel.angleDelta.y > 0)
        image.zoomIn(wheel.x - image.x, wheel.y - image.y);
      else if (wheel.angleDelta.y < 0)
        image.zoomOut(wheel.x - image.x, wheel.y - image.y);
    }
  }

  // Rulers
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
          // We always create as many elements as the image has pixels. We *could* change it according to
          // the zoom value, but that would mean recreating all elements on zooming, which is too expensive.
          model: image.sourceSize.width
          delegate: Rectangle {
            color: "#aaffffff"
            width: 1
            height: index % 10 == 0 ? 10 : 5
            visible: pixelNumber <= image.sourceSize.width // Don't draw the ruler bigger than the image

            // states which pixel of the original scene this bar indicates
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
          // We always create as many elements as the image has pixels. We *could* change it according to
          // the zoom value, but that would mean recreating all elements on zooming, which is too expensive.
          model: image.sourceSize.height
          delegate: Rectangle {
            color: "#aaffffff"
            height: 1
            width: index % 10 == 0 ? 10 : 5
            visible: pixelNumber <= image.sourceSize.height // Don't draw the ruler bigger than the image

            // states which pixel of the original scene this bar indicates
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

  // Zoom buttons
  Row {
    anchors { right: parent.right; top: bottomRuler.top }

    Button {
      width: 20; height: width
      style: buttonStyle
      text: "-"

      onPressedChanged: {
        if (pressed)
          decrementZoomTimer.start();
        else
          decrementZoomTimer.stop();
      }
    }
    Button {
      width: 20; height: width
      style: buttonStyle
      text: "+"

      onPressedChanged: {
        if (pressed)
          incrementZoomTimer.start();
        else
          incrementZoomTimer.stop();
      }
    }
  }

  Timer {
    id: incrementZoomTimer
    interval: 100
    repeat: true
    triggeredOnStart: true
    onTriggered: image.zoomIn(image.width / 2, image.height / 2);
  }
  Timer {
    id: decrementZoomTimer
    interval: 100
    repeat: true
    triggeredOnStart: true
    onTriggered: image.zoomOut(image.width / 2, image.height / 2);
  }
}