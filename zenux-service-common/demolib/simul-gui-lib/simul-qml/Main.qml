import QtQuick 2.14
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.0
import SimulSystemStatus 1.0

ApplicationWindow {
    visible: true
    width: 300
    height: 200
    Material.accent: "#339966"

    CheckBox {
        text: "Schnubbel"
        checked: Simul.schnubbelPlugged
        onCheckedChanged: Simul.schnubbelPlugged = checked
    }
}
