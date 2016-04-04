import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0

Entity {
    id: root

    property Camera camera

    property bool _lmbPressed: false
    property bool _rmbPressed: false
    property bool _zoomIn: false
    property bool _zoomOut: false
    property real _rx: 0
    property real _ry: 0
    property real _tx: 0
    property real _ty: 0

    property vector3d _originalUpVector

    onCameraChanged: {
        if (root.camera) {
            // we don't want to create a binding, but a copy of the initial value
            _originalUpVector = root.camera.upVector;
        }
    }


    KeyboardController {
        id: keyboardController
    }

    MouseController {
        id: mouseController
        sensitivity: 0.1
    }

    LogicalDevice {
        id: logicalDevice;

        actions: [
            Action {
                name: "LeftMouseBtn"
                inputs: [
                    ActionInput {
                        sourceDevice: mouseController
                        keys: [ MouseController.Left ]
                    }
                ]
            },
            Action {
                name: "RightMouseBtn"
                inputs: [
                    ActionInput {
                        sourceDevice: mouseController
                        keys: [ MouseController.Right ]
                    }

                ]
            },
            Action {
                name: "PlusKey"
                inputs: [
                    ActionInput {
                        sourceDevice: keyboardController
                        keys: [ Qt.Key_Plus ]
                    }
                ]
            },
            Action {
                name: "MinusKey";
                inputs: [
                    ActionInput {
                        sourceDevice: keyboardController
                        keys: [ Qt.Key_Minus ]
                    }
                ]
            }
        ]


        axes: [
            Axis {
                name: "RotateX"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseController
                        axis: MouseController.X
                    }
                ]
            },
            Axis {
                name: "RotateY"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseController
                        axis: MouseController.Y
                    }

                ]
            },

            Axis {
                name: "TranslateX"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseController
                        axis: MouseController.X
                    }
                ]
            },
            Axis {
                name: "TranslateY"
                inputs: [
                    AxisInput {
                        sourceDevice: mouseController
                        axis: MouseController.Y
                    }
                ]
            }
        ]
    }

    components: [
        AxisActionHandler {
            id: handler
            logicalDevice: logicalDevice

            onAxisValueChanged: {
                switch(name) {
                case "RotateX":
                    _rx = axisValue;
                    break;
                case "RotateY":
                    _ry = axisValue;
                    break;
                case "TranslateX":
                    _tx = axisValue;
                    break;
                case "TranslateY":
                    _ty = axisValue;
                    break;
                }
            }

            onActionStarted: {
                switch (name) {
                case "LeftMouseBtn":
                    _lmbPressed = true;
                    break;
                case "RightMouseBtn":
                    _rmbPressed = true;
                    break;
                case "PlusKey":
                    _zoomIn = true;
                    break;
                case "MinusKey":
                    _zoomOut = true;
                    break;
                }
            }

            onActionFinished: {
                switch (name) {
                case "LeftMouseBtn":
                    _lmbPressed = false;
                    break;
                case "RightMouseBtn":
                    _rmbPressed = false;
                    break;
                case "PlusKey":
                    _zoomIn = false;
                    break;
                case "MinusKey":
                    _zoomOut = false;
                    break;
                }
            }
        },

        LogicComponent {
            property real translationSpeed : 40.0
            property real rotationSpeed : 180.0
            property real zoomFactor : 0.1

            onFrameUpdate: {
                if (_zoomIn || _zoomOut) {
                    var zoom = 1.0 - zoomFactor;
                    if (_zoomOut) {
                        zoom = 1.0 + zoomFactor;
                    }
                    var viewVector = root.camera.position.minus(root.camera.viewCenter)
                    viewVector = viewVector.times(zoom);
                    root.camera.position = root.camera.viewCenter.plus(viewVector);
                } else if (_lmbPressed) {
                    root.camera.translate(Qt.vector3d(translationSpeed * -_tx,
                                                      translationSpeed * -_ty,
                                                      0).times(dt));
                } else if (_rmbPressed) {
                    root.camera.panAboutViewCenter(rotationSpeed *_rx * dt);
                    root.camera.tiltAboutViewCenter(rotationSpeed * _ry * dt);
                    // lock the camera roll angle
                    root.camera.setUpVector(_originalUpVector);
                }
            }
        }
    ]
}
