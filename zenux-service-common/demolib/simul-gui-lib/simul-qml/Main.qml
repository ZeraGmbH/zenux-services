import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.14
import SimulSystemStatus 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    x: 0
    y: 0
    width: 700
    height: 200
    Material.accent: "#339966"

    ColumnLayout {
        anchors { fill: parent; margins: 10 }
        CheckBox {
            text: "Schnubbel"
            checked: Simul.schnubbelPlugged
            onCheckedChanged: Simul.schnubbelPlugged = checked
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "PLL channel/mode:" }
            Label { text: Simul.pllMode }
        }
        RowLayout {
            enabled: Simul.accuSupported
            Layout.fillWidth: true
            Label { text: "Accu states:" }
            CheckBox {
                text: "present"
                checked: Simul.accuPresent
                onCheckedChanged: Simul.accuPresent = checked
            }
            CheckBox {
                text: "charge"
                checked: Simul.accuIsCharging
                onCheckedChanged: Simul.accuIsCharging = checked
            }
            CheckBox {
                text: "low warning"
                checked: Simul.accuLowWarning
                onCheckedChanged: Simul.accuLowWarning = checked
            }
            CheckBox {
                text: "low alert"
                checked: Simul.accuLowShutdownAlert
                onCheckedChanged: Simul.accuLowShutdownAlert = checked
            }
            CheckBox {
                text: "mains connected"
                checked: Simul.accuMainSupplyPresent
                onCheckedChanged: Simul.accuMainSupplyPresent = checked
            }
        }
        RowLayout {
            enabled: Simul.accuSupported
            Layout.fillWidth: true
            Label { text: "Accu state of charge val:" }
            Label { text: stateOfChargeSlider.value }
            Slider {
                id: stateOfChargeSlider
                from: 0
                to: 110
                stepSize: 1
                Layout.fillWidth: true
                value: Simul.accuStateOfCharge
                onValueChanged: Simul.accuStateOfCharge = value
            }
        }
    }
}
