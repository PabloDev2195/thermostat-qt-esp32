import QtQuick
import QtQuick.Shapes
import QtQuick.Controls

Item {
    id: root

    property real currentTemp: 21.5
    property real targetTemp: 22.0
    property bool heating: false
    property bool connected: true

    property real minTemp: 10.0
    property real maxTemp: 30.0

    property int fanSpeed: 1
    property int thermostatMode: 1

    property bool btConnected: false

    property date now: new Date()

    readonly property real startAngle: 135
    readonly property real sweepAngle: 270

    implicitWidth: mainColumn.implicitWidth
    implicitHeight: mainColumn.implicitHeight

    function angleForTemp(temp) {
        const clamped = Math.max(root.minTemp, Math.min(root.maxTemp, temp));
        const fraction = (clamped - root.minTemp) / (root.maxTemp - root.minTemp);
        return root.startAngle + fraction * root.sweepAngle;
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: root.now = new Date()
    }

    Column {
        id: mainColumn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        spacing: 5

        Text {
            id: dateTimeText
            text: Qt.formatDateTime(root.now, "dddd, d MMMM · hh:mm")
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#f4f4f2"
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        Row {
            id: content
            spacing: 12

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30

                ButtonGroup {
                    id: modeGroup
                }

                Button {
                    text: "OFF"
                    checkable: true
                    checked: true
                    width: 120
                    height: 44
                    ButtonGroup.group: modeGroup

                    background: Rectangle {
                        radius: 6
                        color: "#3a3a3a"
                        border.color: parent.checked ? "#FFFFFF" : "#555555"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.checked ? "black" : "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        root.thermostatMode = 1
                    }
                }

                Button {
                    text: "Auto"
                    checkable: true
                    width: 120
                    height: 44
                    ButtonGroup.group: modeGroup

                    background: Rectangle {
                        radius: 6
                        color: parent.checked ? "#4a90c2" : "#3a3a3a"
                        border.color: parent.checked ? "#FFFFFF" : "#555555"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.checked ? "black" : "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        root.thermostatMode = 2
                    }
                }

                Button {
                    text: "Normal"
                    checkable: true
                    width: 120
                    height: 44
                    ButtonGroup.group: modeGroup

                    background: Rectangle {
                        radius: 6
                        color: parent.checked ? "#4a90c2" : "#3a3a3a"
                        border.color: parent.checked ? "#FFFFFF" : "#555555"
                        border.width: 1
                    }
                    contentItem: Text {
                        text: parent.text
                        color: parent.checked ? "black" : "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        root.thermostatMode = 3
                    }
                }
            }

            Item {
                id: dialArea
                width: 280
                height: 280
                anchors.verticalCenter: parent.verticalCenter

                readonly property real ringRadius: Math.min(width, height) / 2 - 12

                Shape {
                    anchors.fill: parent
                    ShapePath {
                        strokeColor: "#2a2d33"
                        strokeWidth: 14
                        fillColor: "transparent"
                        capStyle: ShapePath.RoundCap
                        PathAngleArc {
                            centerX: dialArea.width / 2
                            centerY: dialArea.height / 2
                            radiusX: dialArea.ringRadius
                            radiusY: dialArea.ringRadius
                            startAngle: root.startAngle
                            sweepAngle: root.sweepAngle
                        }
                    }
                }

                Shape {
                    anchors.fill: parent
                    ShapePath {
                        strokeColor: root.heating ? "#e8964a" : "#4a90c2"
                        strokeWidth: 14
                        fillColor: "transparent"
                        capStyle: ShapePath.RoundCap
                        PathAngleArc {
                            centerX: dialArea.width / 2
                            centerY: dialArea.height / 2
                            radiusX: dialArea.ringRadius
                            radiusY: dialArea.ringRadius
                            startAngle: root.startAngle
                            sweepAngle: root.angleForTemp(root.currentTemp) - root.startAngle
                        }
                    }
                }

                Rectangle {
                    width: 30
                    height: 30
                    radius: 15
                    color: "#f4f4f2"
                    border.color: "#1b1d21"
                    border.width: 4
                    x: dialArea.width / 2
                       + dialArea.ringRadius * Math.cos(root.angleForTemp(root.targetTemp) * Math.PI / 180)
                       - width / 2
                    y: dialArea.height / 2
                       + dialArea.ringRadius * Math.sin(root.angleForTemp(root.targetTemp) * Math.PI / 180)
                       - height / 2
                }

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: root.currentTemp.toFixed(1) + "°"
                        color: "#f4f4f2"
                        font.pixelSize: 48
                        font.weight: Font.Medium
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Target " + root.targetTemp.toFixed(1) + "°"
                        color: "#9a9a92"
                        font.pixelSize: 14
                    }
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 6
                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            anchors.verticalCenter: parent.verticalCenter
                            color: root.connected ? "#63a35c" : "#a33d3d"
                        }
                        Text {
                            text: root.connected ? (root.heating ? "Heating" : "Idle") : "Disconnected"
                            color: "#9a9a92"
                            font.pixelSize: 12
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onPositionChanged: (mouse) => {
                        if (pressed) {
                            const dx = mouse.x - dialArea.width / 2;
                            const dy = mouse.y - dialArea.height / 2;
                            let angleDeg = Math.atan2(dy, dx) * 180 / Math.PI;
                            if (angleDeg < 0) angleDeg += 360;
                            let normalized = angleDeg - root.startAngle;
                            while (normalized < 0) normalized += 360;
                            const fraction = Math.max(0, Math.min(1, normalized / root.sweepAngle));
                            const newTemp = Math.round((root.minTemp + fraction * (root.maxTemp - root.minTemp)) * 2) / 2;
                            if (newTemp !== root.targetTemp) {
                                root.targetTemp = newTemp;
                            }
                        }
                    }
                }

            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 4

                    Repeater {
                        model: 3
                        delegate: Rectangle {
                            width: 12
                            height: 12 + index * 12
                            radius: 5
                            color: (index < root.fanSpeed) ? "#4a90c2" : "#3a3d43"
                            anchors.bottom: parent.bottom
                        }
                    }
                }

                Rectangle {
                    id: fanButton
                    width: 70
                    height: 70
                    radius: 35
                    color: "#2a2d33"
                    border.color: "#3a3d43"
                    border.width: 1

                    Item {
                        id: fanIcon
                        anchors.centerIn: parent
                        width: 40
                        height: 40

                        Shape {
                            anchors.fill: parent
                            transform: Scale {
                                xScale: fanIcon.width / 512
                                yScale: fanIcon.height / 512
                            }
                            ShapePath {
                                fillColor: "#4a90c2"
                                strokeColor: "transparent"
                                PathSvg {
                                    path: "M258.6 0c-1.7 0-3.4 .1-5.1 .5C168 17 115.6 102.3 130.5 189.3c2.9 17 8.4 32.9 15.9 47.4L32 224H29.4C13.2 224 0 237.2 0 253.4c0 1.7 .1 3.4 .5 5.1C17 344 102.3 396.4 189.3 381.5c17-2.9 32.9-8.4 47.4-15.9L224 480v2.6c0 16.2 13.2 29.4 29.4 29.4c1.7 0 3.4-.1 5.1-.5C344 495 396.4 409.7 381.5 322.7c-2.9-17-8.4-32.9-15.9-47.4L480 288h2.6c16.2 0 29.4-13.2 29.4-29.4c0-1.7-.1-3.4-.5-5.1C495 168 409.7 115.6 322.7 130.5c-17 2.9-32.9 8.4-47.4 15.9L288 32V29.4C288 13.2 274.8 0 258.6 0zM256 288c-17.7 0-32-14.3-32-32s14.3-32 32-32s32 14.3 32 32s-14.3 32-32 32z"
                                }
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.fanSpeed = (root.fanSpeed % 3) + 1;
                        }
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Fan"
                    color: "#f4f4f2"
                    font.pixelSize: 20
                    font.weight: Font.DemiBold
                }
            }
        }
    }

    Shape {
        id: bluetoothIcon
        anchors.verticalCenter: dateTimeText.verticalCenter
        anchors.right: parent.right
        anchors.margins: 16
        width: 30
        height: 30

        ShapePath {
            strokeWidth: 2
            strokeColor: root.btConnected ? "#4da6ff" : "#666666"
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin

            startX: 6; startY: 4
            PathLine { x: 18; y: 16 }
            PathLine { x: 12; y: 22 }
            PathLine { x: 12; y: 2 }
            PathLine { x: 18; y: 8 }
            PathLine { x: 6; y: 20 }
        }
    }
}
