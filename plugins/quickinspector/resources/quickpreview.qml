/*
  quickpreview.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import com.kdab.GammaRay 1.0

Image {
  id: root
  source: "image://quicksceneprovider/background"
  fillMode: Image.Tile
  property variant previewData: {}
  property bool isFirstFrame: true
  property bool supportsCustomRenderModes: true

  focus: true

  Keys.onPressed: { // event-forwarding
    if (mouseToolGroup.current == moveModeButton && event.modifiers == Qt.ControlModifier) {
      mouseToolGroup.current = measureModeButton;
      mouseToolGroup.modifierDriven = true;
    } else if (mouseToolGroup.current == moveModeButton && event.modifiers == Qt.ShiftModifier) {
      mouseToolGroup.current = redirectInputModeButton;
      mouseToolGroup.modifierDriven = true;
    } else {
      inspectorInterface.sendKeyEvent(6, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
    }
  }
  Keys.onReleased: { // event-forwarding
    if (mouseToolGroup.modifierDriven == true && mouseToolGroup.current == measureModeButton && event.key == Qt.Key_Control
        || mouseToolGroup.modifierDriven == true && mouseToolGroup.current == redirectInputModeButton && event.key == Qt.Key_Shift
        || event.key == Qt.Key_Escape) {
      mouseToolGroup.current = moveModeButton;
      mouseToolGroup.modifierDriven = false;
    } else {
      inspectorInterface.sendKeyEvent(7, event.key, event.modifiers, event.text, event.isAutoRepeat, event.count);
    }
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
    border.color: "grey"
    width: toolbarRow.width + 8; height: toolbarRow.height
    x: 5; y: 5; z: 1
    radius: 4
    visible: toolbarRow.width

    Row {
      id: toolbarRow
      x: 4

      ExclusiveGroup {
        id: renderModeGroup
        property QtObject oldCurrent
      }
      ExclusiveGroup {
        id: mouseToolGroup
        property bool modifierDriven: false
      }

      ToolButton {
        id: clippingButton
        height: 20
        visible: supportsCustomRenderModes
        exclusiveGroup: renderModeGroup
        checkable: true

        iconSource: "qrc:///gammaray/plugins/quickinspector/visualize-clipping.png"
        tooltip: qsTr("<b>Visualize Clipping</b><br/>"
               + "Items with the property <i>clip</i> set to true, will cut off their and their "
               + "children's rendering at the items' bounds. While this is a handy feature it "
               + "comes with quite some cost, like disabling some performance optimizations.<br/>"
               + "With this tool enabled the QtQuick renderer highlights items, that have clipping "
               + "enabled, so you can check for items, that have clipping enabled unnecessarily. "
               + "<br/><br/><i>Note: The visualization happens on the application, not in the "
               + "GammaRay preview panel.</i>")

        onClicked: {
          if (renderModeGroup.oldCurrent == clippingButton)
            renderModeGroup.current = null;
          inspectorInterface.setCustomRenderMode(checked ? QuickInspectorInterface.VisualizeClipping : QuickInspectorInterface.NormalRendering);
          renderModeGroup.oldCurrent = renderModeGroup.current;
        }
      }
      ToolButton {
        id: overdrawButton
        height: 20
        visible: supportsCustomRenderModes
        exclusiveGroup: renderModeGroup
        checkable: true

        iconSource: "qrc:///gammaray/plugins/quickinspector/visualize-overdraw.png"
        tooltip: qsTr("<b>Visualize Overdraw</b><br/>"
               + "The QtQuick renderer doesn't detect if an item is obscured by another "
               + "opaque item, is completely outside the scene or outside a clipped ancestor and "
               + "thus doesn't need to be rendered. You thus need to take care of setting "
               + "<i>visible: false</i> for hidden items, yourself.<br/>"
               + "With this tool enabled the QtQuick renderer draws a 3D-Box visualizing the "
               + "layers of items that are drawn."
               + "<br/><br/><i>Note: The visualization happens on the application, not in the "
               + "GammaRay preview panel.</i>")

        onClicked: {
          if (renderModeGroup.oldCurrent == overdrawButton)
            renderModeGroup.current = null;
          inspectorInterface.setCustomRenderMode(checked ? QuickInspectorInterface.VisualizeOverdraw : QuickInspectorInterface.NormalRendering);
          renderModeGroup.oldCurrent = renderModeGroup.current;
        }
      }
      ToolButton {
        id: batchesButton
        height: 20
        visible: supportsCustomRenderModes
        exclusiveGroup: renderModeGroup
        checkable: true

        iconSource: "qrc:///gammaray/plugins/quickinspector/visualize-batches.png"
        tooltip: qsTr("<b>Visualize Batches</b><br/>"
               + "Where a traditional 2D API, such as QPainter, Cairo or Context2D, is written to "
               + "handle thousands of individual draw calls per frame, OpenGL is a pure hardware "
               + "API and performs best when the number of draw calls is very low and state "
               + "changes are kept to a minimum. Therefore the QtQuick renderer combines the "
               + "rendering of similar items into single batches.<br/>"
               + "Some settings (like <i>clip: true</i>) will cause the batching to fail, though, "
               + "causing items to be rendered separately. With this tool enabled the QtQuick "
               + "renderer visualizes those batches, by drawing all items that are batched using "
               + "the same color. The fewer colors you see in this mode the better."
               + "<br/><br/><i>Note: The visualization happens on the application, not in the "
               + "GammaRay preview panel.</i>")

        onClicked:  {
          if (renderModeGroup.oldCurrent == batchesButton)
            renderModeGroup.current = null;
          inspectorInterface.setCustomRenderMode(checked ? QuickInspectorInterface.VisualizeBatches : QuickInspectorInterface.NormalRendering);
          renderModeGroup.oldCurrent = renderModeGroup.current;
        }
      }
      ToolButton {
        id: changesButton
        height: 20
        visible: supportsCustomRenderModes
        exclusiveGroup: renderModeGroup
        checkable: true

        iconSource: "qrc:///gammaray/plugins/quickinspector/visualize-changes.png"
        tooltip: qsTr("<b>Visualize Changes</b><br>"
               + "The QtQuick scene is only repainted, if some item changes in a visual manner. "
               + "Unnecessary repaints can have a bad impact on the performance. With this tool "
               + "enabled, the QtQuick renderer will thus on each repaint highlight the item(s), "
               + "that caused the repaint."
               + "<br/><br/><i>Note: The visualization happens on the application, not in the "
               + "GammaRay preview panel.</i>")

        onClicked: {
          if (renderModeGroup.oldCurrent == changesButton)
            renderModeGroup.current = null;
          inspectorInterface.setCustomRenderMode(checked ? QuickInspectorInterface.VisualizeChanges : QuickInspectorInterface.NormalRendering);
          renderModeGroup.oldCurrent = renderModeGroup.current;
        }
      }

      Item {
        width: 5; height: parent.height - 4
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
          width: 1; height: parent.height
          anchors.horizontalCenter: parent.horizontalCenter
          color: "grey"
        }
      }
      ToolButton {
        id: moveModeButton
        height: 20
        checkable: true
        checked: true
        exclusiveGroup: mouseToolGroup

        iconSource: "qrc:///gammaray/plugins/quickinspector/move-preview.png"
        tooltip: qsTr("<b>Move preview</b><br>"
               + "Default mode. Click and drag to move the preview. Won't impact the original "
               + "application in any way. ")
      }
      ToolButton {
        id: measureModeButton
        height: 20
        checkable: true
        exclusiveGroup: mouseToolGroup

        iconSource: "qrc:///gammaray/plugins/quickinspector/measure-pixels.png"
        tooltip: qsTr("<b>Measure pixel-sizes</b> (Ctrl)<br>"
               + "Choose this mode, click somewhere and drag to measure the distance between the "
               + "point you clicked and the point where your mouse pointer is. (Measured in scene "
               + "coordinates).")
      }
      ToolButton {
        id: redirectInputModeButton
        height: 20
        checkable: true
        exclusiveGroup: mouseToolGroup

        iconSource: "qrc:///gammaray/plugins/quickinspector/redirect-input.png"
        tooltip: qsTr("<b>Redirect Input</b> (Shift)<br>"
               + "In this mode all mouse input is redirected directly to the original application,"
               + "so you can control the application directly from within GammaRay.")
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
    }
  }

  // Scene preview
  Flickable {
    id: sceneFlickable
    anchors.fill: parent
    contentWidth: image.width
    contentHeight: image.height
    boundsBehavior: Flickable.StopAtBounds

    AnnotatedScenePreview {
      id: image
      anchors.centerIn: parent
      previewData: root.previewData
      margin { width: root.width; height: root.height }

      onPreviewDataChanged: {
        // Align image to center
        if (isFirstFrame) {
          sceneFlickable.contentX = -(root.width - sceneFlickable.contentWidth - rightRuler.width) / 2;
          sceneFlickable.contentY = -(root.height - sceneFlickable.contentHeight - bottomRuler.height) / 2;
          isFirstFrame = false;
        }
      }

      function zoomIn(zoomToX, zoomToY) {
        var oldZoom = zoom;
        zoom = zoom < 1
              ? 1 / (1 / zoom - 1)
              : zoom + 1;
        sceneFlickable.contentX -= zoomToX * (1 - zoom / oldZoom);
        sceneFlickable.contentY -= zoomToY * (1 - zoom / oldZoom);
      }
      function zoomOut(zoomToX, zoomToY) {
        var oldZoom = zoom;
        zoom = zoom <= 1
              ? 1 / (1 / zoom + 1)
              : zoom - 1;
        sceneFlickable.contentX -= zoomToX * (1 - zoom / oldZoom);
        sceneFlickable.contentY -= zoomToY * (1 - zoom / oldZoom);
      }

      Canvas {
        id: canvas
        anchors.centerIn: parent
        width: parent.width - parent.margin.width
        height: parent.height - parent.margin.height
        property point start
        property point end

        onPaint: {
          var ctx = getContext("2d");
          ctx.reset();
          if (start == end)
            return;

          ctx.lineWidth = 1;
          ctx.beginPath();

          ctx.moveTo(start.x - 5, start.y);
          ctx.lineTo(start.x + 5, start.y);
          ctx.moveTo(start.x, start.y - 5);
          ctx.lineTo(start.x, start.y + 5);

          ctx.moveTo(start.x, start.y);
          ctx.lineTo(end.x, end.y);

          ctx.moveTo(end.x - 5, end.y);
          ctx.lineTo(end.x + 5, end.y);
          ctx.moveTo(end.x, end.y - 5);
          ctx.lineTo(end.x, end.y + 5);

          ctx.stroke();
          ctx.closePath();
        }
      }

      MouseArea {
        id: imageMA
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true


        onReleased: { // event-forwarding
          canvas.start = canvas.end = Qt.point(0,0);
          canvas.requestPaint();
          if (mouseToolGroup.current == redirectInputModeButton)
            inspectorInterface.sendMouseEvent(3, Qt.point((mouse.x - image.margin.width / 2) / image.zoom, (mouse.y - image.margin.height / 2) / image.zoom),
                                              mouse.button, mouse.buttons, mouseToolGroup.modifierDriven ? mouse.modifiers & ~Qt.ShiftModifier : mouse.modifiers);
          else
            mouse.accepted = false;
        }
        onPressed: { // event-forwarding / measure
          if (mouseToolGroup.current == redirectInputModeButton)
            inspectorInterface.sendMouseEvent(2, Qt.point((mouse.x - image.margin.width / 2) / image.zoom, (mouse.y - image.margin.height / 2) / image.zoom),
                                              mouse.button, mouse.buttons, mouseToolGroup.modifierDriven ? mouse.modifiers & ~Qt.ShiftModifier : mouse.modifiers);
          else if (mouseToolGroup.current == measureModeButton)
            canvas.start = Qt.point(Math.round((mouse.x - image.margin.width / 2) / image.zoom) * image.zoom, Math.round((mouse.y - image.margin.height / 2) / image.zoom) * image.zoom);
          else
            mouse.accepted = false;
        }
        onPositionChanged: { // event-forwarding / measure
          if (mouseToolGroup.current == redirectInputModeButton) {
            inspectorInterface.sendMouseEvent(5, Qt.point((mouse.x - image.margin.width / 2) / image.zoom, (mouse.y - image.margin.height / 2) / image.zoom),
                                              mouse.button, mouse.buttons, mouseToolGroup.modifierDriven ? mouse.modifiers & ~Qt.ShiftModifier : mouse.modifiers);
          } else if (pressed && mouseToolGroup.current == measureModeButton) {
            canvas.end = Qt.point(Math.round((mouse.x - image.margin.width / 2) / image.zoom) * image.zoom, Math.round((mouse.y - image.margin.height / 2) / image.zoom) * image.zoom);
            overlayText.text = "(" + Math.floor(canvas.start.x / image.zoom) + ", " + Math.floor(canvas.start.y / image.zoom) + ") - ("
                  + Math.floor(canvas.end.x / image.zoom) + ", " + Math.floor(canvas.end.y / image.zoom) + ") -> "
                  + (Math.sqrt( Math.pow(canvas.end.x - canvas.start.x, 2) + Math.pow(canvas.end.y - canvas.start.y, 2) ) / image.zoom).toFixed(3) + "px";
            canvas.requestPaint();
          } else {
            overlayText.text = Math.floor((mouse.x - image.margin.width / 2) / image.zoom) + ", " + Math.floor((mouse.y - image.margin.height / 2) / image.zoom);
            mouse.accepted = false;
          }
        }
        onDoubleClicked: { // event-forwarding
          if (mouseToolGroup.current == redirectInputModeButton)
            inspectorInterface.sendMouseEvent(4, Qt.point((mouse.x - image.margin.width / 2) / image.zoom, (mouse.y - image.margin.height / 2) / image.zoom),
                                              mouse.button, mouse.buttons, mouseToolGroup.modifierDriven ? mouse.modifiers & ~Qt.ShiftModifier : mouse.modifiers);
          else
            mouse.accepted = false;
        }
        onWheel: { // event-forwarding / zoom
          if (mouseToolGroup.current == redirectInputModeButton)
            inspectorInterface.sendWheelEvent(Qt.point((wheel.x - image.margin.width / 2) / image.zoom, (wheel.y - image.margin.height / 2) / image.zoom),
                                              wheel.pixelDelta, wheel.angleDelta, wheel.buttons, mouseToolGroup.modifierDriven ? wheel.modifiers & ~Qt.ShiftModifier : wheel.modifiers);
          else if (wheel.angleDelta.y > 0) {
            var point = mapToItem(image, (wheel.x - image.margin.width / 2), (wheel.y - image.margin.height / 2));
            image.zoomIn(point.x, point.y);
          } else if (wheel.angleDelta.y < 0) {
            var point = mapToItem(image, (wheel.x - image.margin.width / 2), (wheel.y - image.margin.height / 2));
            image.zoomOut(point.x, point.y);
          } else
            wheel.accepted = false;
        }
      }
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
        x: sceneFlickable.width / 2 - sceneFlickable.contentX
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
        y: sceneFlickable.height / 2 - sceneFlickable.contentY
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

    ToolButton {
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
    ToolButton {
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
