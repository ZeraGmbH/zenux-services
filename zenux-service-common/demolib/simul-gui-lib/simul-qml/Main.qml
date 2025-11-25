import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.14
import SimulSystemStatus 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    title: "PCB service simulation"
    x: 0
    y: 0
    width: 1280
    height: 300
    Material.accent: "#339966"
    readonly property real labelWidth: width * 0.15
    readonly property real channelWidth: (width - 60 - labelWidth) / Simul.channelRanges.length

    ColumnLayout {
        anchors { fill: parent; margins: 10 }
        CheckBox {
            text: "Schnubbel"
            checked: Simul.schnubbelPlugged
            onCheckedChanged: Simul.schnubbelPlugged = checked
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "PLL channel/mode:"; Layout.preferredWidth: labelWidth }
            Label { text: Simul.pllMode }
        }
        RowLayout {
            enabled: Simul.accuSupported
            Layout.fillWidth: true
            Label { text: "Accu states:"; Layout.preferredWidth: labelWidth }
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
            Label { text: "Accu state of charge val:"; Layout.preferredWidth: labelWidth }
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
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Ranges:"; Layout.preferredWidth: labelWidth }
            Repeater {
                model: Simul.channelRanges.length
                Label { text: Simul.channelRanges[index]; Layout.preferredWidth: channelWidth }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: "Hotplug:"; Layout.preferredWidth: labelWidth }
            Repeater {
                model: Simul.channelRanges.length
                ComboBox {
                    id: hotDevicesCombo
                    enabled: Simul.channelHotplugSupported[index]
                    opacity: Simul.channelHotplugSupported[index] ? 1 : 0
                    readonly property int channelIdx: index
                    Layout.preferredWidth: channelWidth
                    model: Simul.hotplugDevices
                    onCurrentIndexChanged: Simul.changeHotplugDevice(channelIdx, currentIndex)

                    contentItem: Label {
                        text: hotDevicesCombo.displayText
                        font.pointSize: hotDevicesCombo.height*0.2
                        verticalAlignment: Text.AlignVCenter
                        leftPadding: width * 0.1
                        rightPadding: leftPadding
                    }
                    popup: Popup {
                        x: -width*0.3
                        width: hotDevicesCombo.width * 1.5
                        height: Math.min(contentItem.implicitHeight, mainWindow.height - topMargin - bottomMargin)
                        padding: 1
                        contentItem: ListView {
                            clip: true
                            implicitHeight: contentHeight * 0.8
                            model: hotDevicesCombo.popup.visible ? hotDevicesCombo.delegateModel : null
                            currentIndex: hotDevicesCombo.highlightedIndex
                            ScrollIndicator.vertical: ScrollIndicator { }
                        }
                    }
                }
            }
        }
    }
}
