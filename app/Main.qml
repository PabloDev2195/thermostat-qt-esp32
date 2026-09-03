import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow {
    id: window
    width: 600
    height: 400
    minimumWidth: 200
    minimumHeight: 250
    visible: true
    title: qsTr("Smart Home")
    property bool lightMode: Application.styleHints.colorScheme === Qt.Light
    property color reallyDark: "#1f1f1f"
    property color dark: "#262626"
    property color reallyLight: "#e7e7e7"
    property color light: "#e0e0e0"

    ThermostatDial {
        anchors.centerIn: parent
    }

}
