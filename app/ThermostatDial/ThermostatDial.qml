import QtQuick
import QtQuick.Shapes
import QtQuick.Controls
import Thermostat 1.0

/**
 * ThermostatDial
 * ----------------
 * Main thermostat view: Nest-style circular dial, mode controls
 * (Off/Auto/Normal), fan control, clock/date, and BLE connection status
 * with the ESP32.
 *
 * High-level hierarchy:
 *  - BleManager (C++ backend): handles the actual BLE scanning/connection.
 *  - mainColumn: date/time + row of controls (modes | dial | fan).
 *  - bluetoothIcon: visual BLE connection indicator (color reflects state).
 *
 * NOTE: there are currently two separate sources of "connected" state:
 *  - `root.connected` (local bool, currently hardcoded to true) drives the
 *    status text ("Idle"/"Heating"/"Disconnected") inside the dial.
 *  - `bleManager.connected` (real backend property) drives the color of
 *    the Bluetooth icon.
 *  These two are not yet unified — see comment on `root.connected`.
 */
Item {
    id: root

    // ------------------------------------------------------------------
    // BLE backend
    // ------------------------------------------------------------------

    /**
     * Instance of the C++ backend (BleManager) exposed to QML as type
     * "Thermostat 1.0". Once the component finishes creation, it
     * automatically starts a BLE scan looking for the ESP32 ("nimble-bleprph").
     */
    BleManager {
        id: bleManager
        Component.onCompleted: startScan()
    }

    // ------------------------------------------------------------------
    // Thermostat state (UI data / placeholders)
    // ------------------------------------------------------------------

    /// Current ambient temperature shown on the dial. Placeholder: 21.5°.
    property real currentTemp: 21.5

    /// Target temperature the user adjusts by dragging the dial handle.
    property real targetTemp: 22.0

    /// Whether the system is currently heating (changes the active arc color).
    property bool heating: false

    /**
     * Connection state shown in the dial's center status text ("Idle" /
     * "Heating" / "Disconnected") and its accompanying colored dot.
     * TODO: currently hardcoded to `true`; should be bound to
     * `bleManager.connected` once the Qt<->ESP32 connection logic is
     * complete (see the Qt<->ESP32 connection ticket).
     */
    property bool connected: true

    /// Lower bound of the temperature range representable on the dial.
    property real minTemp: 10.0

    /// Upper bound of the temperature range representable on the dial.
    property real maxTemp: 30.0

    /// Current fan speed, 1 to 3 (controls how many bars of the fan icon light up).
    property int fanSpeed: 1

    /// Thermostat mode: 1 = Off, 2 = Auto, 3 = Normal (see the mode ButtonGroup).
    property int thermostatMode: 1

    /// Current date/time, refreshed every second by the Timer below; feeds the date text.
    property date now: new Date()

    /// Angle (in degrees) where the dial's arc begins (roughly the "10 o'clock" position).
    readonly property real startAngle: 135

    /// Total sweep of the dial's arc, in degrees (leaves a gap at the bottom, Nest-style).
    readonly property real sweepAngle: 270

    implicitWidth: mainColumn.implicitWidth
    implicitHeight: mainColumn.implicitHeight

    /**
     * Converts a temperature value to its corresponding angle on the dial's arc.
     * @param temp Temperature to convert; clamped to [minTemp, maxTemp].
     * @return Angle in degrees within [startAngle, startAngle + sweepAngle].
     */
    function angleForTemp(temp) {
        const clamped = Math.max(root.minTemp, Math.min(root.maxTemp, temp));
        const fraction = (clamped - root.minTemp) / (root.maxTemp - root.minTemp);
        return root.startAngle + fraction * root.sweepAngle;
    }

    /// Refreshes `root.now` every second to keep the UI clock up to date.
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: root.now = new Date()
    }

    // ------------------------------------------------------------------
    // Main layout
    // ------------------------------------------------------------------

    /// Root column: date/time on top, row of controls (modes | dial | fan) below.
    Column {
        id: mainColumn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        spacing: 5

        /// Date and time text, formatted as "dddd, d MMMM · hh:mm".
        Text {
            id: dateTimeText
            text: Qt.formatDateTime(root.now, "dddd, d MMMM · hh:mm")
            anchors.horizontalCenter: parent.horizontalCenter
            color: "#f4f4f2"
            font.pixelSize: 20
            font.weight: Font.DemiBold
        }

        /// Row holding the three main sections: mode buttons, dial, fan control.
        Row {
            id: content
            spacing: 12

            // ----------------------------------------------------------
            // Mode buttons column (Off / Auto / Normal)
            // ----------------------------------------------------------

            /// Thermostat mode buttons, mutually exclusive via ButtonGroup.
            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30

                /// Groups the three mode buttons so only one is "checked" at a time.
                ButtonGroup {
                    id: modeGroup
                }

                /// "OFF" mode button (thermostatMode = 1). Checked by default on startup.
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

                /// "Auto" mode button (thermostatMode = 2).
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

                /// "Normal" mode button (thermostatMode = 3).
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

            // ----------------------------------------------------------
            // Circular dial (the thermostat's visual centerpiece)
            // ----------------------------------------------------------

            /// Dial container: background track, active arc, target handle, and center text.
            Item {
                id: dialArea
                width: 280
                height: 280
                anchors.verticalCenter: parent.verticalCenter

                /// Radius of the dial's ring, derived from the container's size.
                readonly property real ringRadius: Math.min(width, height) / 2 - 12

                /// Background track of the dial (full arc in dark gray, always visible).
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

                /**
                 * Active arc: represents the current temperature over the background track.
                 * Colored orange when `heating` is active, blue otherwise.
                 * The sweep goes from `startAngle` to the angle for `currentTemp`.
                 */
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

                /**
                 * Handle (knob) marking the `targetTemp` position on the ring.
                 * Its (x, y) position is computed trigonometrically from the angle
                 * corresponding to the target, using the ring's radius.
                 */
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

                /// Center content of the dial: current temperature, target, and connection status.
                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    /// Current temperature, large, centered in the dial (e.g. "21.5°").
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: root.currentTemp.toFixed(1) + "°"
                        color: "#f4f4f2"
                        font.pixelSize: 48
                        font.weight: Font.Medium
                    }
                    /// Secondary text showing the target temperature.
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Target " + root.targetTemp.toFixed(1) + "°"
                        color: "#9a9a92"
                        font.pixelSize: 14
                    }
                    /// Status indicator: colored dot + text ("Idle"/"Heating"/"Disconnected").
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 6
                        /// Colored dot: green if `root.connected`, red otherwise.
                        Rectangle {
                            width: 8
                            height: 8
                            radius: 4
                            anchors.verticalCenter: parent.verticalCenter
                            color: root.connected ? "#63a35c" : "#a33d3d"
                        }
                        /// Status text derived from `connected` and `heating`.
                        Text {
                            text: root.connected ? (root.heating ? "Heating" : "Idle") : "Disconnected"
                            color: "#9a9a92"
                            font.pixelSize: 12
                        }
                    }
                }

                /**
                 * Mouse/touch area covering the whole dial to allow dragging the
                 * target handle. Converts cursor position into an angle, then into
                 * a fraction of the [minTemp, maxTemp] range, rounding to 0.5° steps.
                 */
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

            // ----------------------------------------------------------
            // Fan control
            // ----------------------------------------------------------

            /// Column with speed indicator, circular fan button, and label.
            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8

                /// Three increasing-height bars visually indicating `fanSpeed` (1 to 3 lit).
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

                /// Circular button with a fan icon; clicking cycles fanSpeed 1→2→3→1.
                Rectangle {
                    id: fanButton
                    width: 70
                    height: 70
                    radius: 35
                    color: "#2a2d33"
                    border.color: "#3a3d43"
                    border.width: 1

                    /// Fan SVG icon, scaled to fit the button.
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

                    /// On click, cycles `fanSpeed`: 1 → 2 → 3 → 1.
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.fanSpeed = (root.fanSpeed % 3) + 1;
                        }
                    }
                }

                /// Text label below the fan button.
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

    // ------------------------------------------------------------------
    // Bluetooth connection indicator
    // ------------------------------------------------------------------

    /**
     * Bluetooth icon (hand-drawn with PathLine, classic BT logo shape).
     * Color reflects `bleManager.connected`:
     * blue (#4da6ff) when a BLE connection is active, gray (#666666) otherwise.
     * Positioned in the top-right corner, aligned with the date/time text.
     */
    Shape {
        id: bluetoothIcon
        anchors.verticalCenter: dateTimeText.verticalCenter
        anchors.right: parent.right
        anchors.margins: 16
        width: 30
        height: 30

        ShapePath {
            strokeWidth: 2
            strokeColor: bleManager.connected ? "#4da6ff" : "#666666"
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
