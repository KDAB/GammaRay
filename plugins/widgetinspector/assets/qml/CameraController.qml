import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property Camera camera

    property vector3d _originalUpVector

    onCameraChanged: {
        if (root.camera) {
            // we don't want to create a binding, but a copy of the initial value
            _originalUpVector = root.camera.upVector;
        }
    }


    KeyboardDevice {
        id: keyboard
    }

    MouseDevice {
        id: mouse
        sensitivity: 0.1
    }

    components: [
        LogicalDevice {
              id: logicalDevice;

              actions: [
                  Action {
                      id: moveAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              buttons: [ MouseEvent.LeftButton ]
                          }
                      ]
                  },
                  Action {
                      id: rotateAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              buttons: [ MouseEvent.RightButton ]
                          }

                      ]
                  },
                  Action {
                      id: zoomInAction
                      inputs: [
                          ActionInput {
                              sourceDevice: keyboard
                              buttons: [ Qt.Key_Plus ]
                          }
                      ]
                  },
                  Action {
                      id: zoomOutAction
                      inputs: [
                          ActionInput {
                              sourceDevice: keyboard
                              buttons: [ Qt.Key_Minus ]
                          }
                      ]
                  }
              ]


              axes: [
                  Axis {
                      id: rotateXAxis
                      inputs: [
                          AxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: rotateYAxis
                      inputs: [
                          AxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }

                      ]
                  },

                  Axis {
                      id: translateXAxis
                      inputs: [
                          AxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: translateYAxis
                      inputs: [
                          AxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }
                      ]
                  }
              ]
          },

        FrameAction {
            property real translationSpeed : 80.0
            property real rotationSpeed : 180.0
            property real zoomFactor : 0.1

            onTriggered: {
                if (zoomInAction.active || zoomOutAction.active) {
                    var zoom = 1.0 - zoomFactor;
                    if (zoomOutAction.active) {
                        zoom = 1.0 + zoomFactor;
                    }
                    var viewVector = root.camera.position.minus(root.camera.viewCenter)
                    viewVector = viewVector.times(zoom);
                    root.camera.position = root.camera.viewCenter.plus(viewVector);
                } else if (moveAction.active) {
                    root.camera.translate(Qt.vector3d(translationSpeed * -translateXAxis.value,
                                                      translationSpeed * -translateYAxis.value,
                                                      0).times(dt));
                } else if (rotateAction.active) {
                    root.camera.panAboutViewCenter(rotationSpeed * rotateXAxis.value * dt);
                    root.camera.tiltAboutViewCenter(rotationSpeed * rotateYAxis.value * dt);
                    // lock the camera roll angle
                    root.camera.setUpVector(_originalUpVector);
                }
            }
        }
    ]
}
