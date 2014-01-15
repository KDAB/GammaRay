import QtQuick 2.0

Rectangle {
  color: "lightsteelblue"
  width: 640
  height: 480
  focus: true

  Text {
    id: label
    anchors.centerIn: parent
    text: "press a key"
  }

  // always works
  Keys.onLeftPressed: { label.text = "left pressed"; }

  // breaks with signal spy callbacks installed
  Keys.onPressed: { label.text = "key pressed"; console.log("xxx"); }
}
