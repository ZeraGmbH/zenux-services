import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
import SimulSystemStatus 1.0

Window {
    id: root
    visible: true

    width: 300
    height: 200
    CheckBox {
        text: "Schnubbel"
        checked: Simul.schnubbelPlugged
        onCheckedChanged: Simul.schnubbelPlugged = checked
    }
}
