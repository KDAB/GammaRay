/*
  state-machine.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.5
import QtQuick.Window 2.0
import QtQml.StateMachine 1.0

Rectangle {
    width: 120
    height: 4*width
    color: "lightsteelblue"

    Rectangle {
        id: brokenTrafficLight
        color: "black"
        width: parent.width
        height: 3*width

        Column {
            anchors.fill: parent
            anchors.margins: spacing
            spacing: 5

            Rectangle {
                id: redLight
                color: red.active || redToGreen.active ? "red" : "gray"
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                radius: width/2
            }

            Rectangle {
                id: yellowLight
                color: redToGreen.active || greenToRed.active ? "yellow" : "gray"
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                radius: width/2
            }

            Rectangle {
                id: greenLight
                color: green.active ? "green" : "gray"
                anchors.left: parent.left
                anchors.right: parent.right
                height: width
                radius: width/2
            }
        }
    }

    Rectangle {
        id: stand
        color: brokenTrafficLight.color
        width: brokenTrafficLight.width / 3
        anchors.top: brokenTrafficLight.bottom
        anchors.horizontalCenter: brokenTrafficLight.horizontalCenter
        anchors.bottom: parent.bottom

        Rectangle {
            id: buttonBase
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            height: width
            color: "gray"

            MouseArea {
                id: onOffSwitch
                anchors.fill: parent
            }

            Rectangle {
                id: button
                color: "cyan"
                width: parent.width
                height: parent.height/2
                y: off.active ? height : 0
            }
        }
    }

    StateMachine {
        id: brokenTrafficLightMachine
        running: true
        initialState: running

        State {
            id: running
            initialState: red

            State {
                id: red

                TimeoutTransition {
                    timeout: 5000
                    targetState: redToGreen
                }
            }

            State {
                id: redToGreen

                TimeoutTransition {
                    timeout: 1500
                    targetState: green
                }
            }

            //! [Wrong transition]
            State {
                id: green

                TimeoutTransition {
                    timeout: 5000
                    targetState: red
                }
            }
            //! [Wrong transition]

            State {
                id: greenToRed

                TimeoutTransition {
                    timeout: 1500
                    targetState: red
                }
            }

            HistoryState {
                id: pause
                defaultState: red
            }

            SignalTransition {
                signal: onOffSwitch.clicked
                targetState: off
            }
        }

        State {
            id: off

            SignalTransition {
                signal: onOffSwitch.clicked
                targetState: pause
            }
        }
    }
}
