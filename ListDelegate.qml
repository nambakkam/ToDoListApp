import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.0

// ListDelegate.qml: Represents a single item in a list with a checkbox, task name, and remove button
Rectangle {
    width: parent.width
    height: width * 0.15

    // Properties for item data and UI control
    property string serialNumber: "1"         // Serial number of the task
    property string taskName: "taskName"      // Name of the task
    property bool completionStatus: false     // Completion status of the task

    // Aliases for accessing child elements externally
    property alias checkBoxComp: completionStatusBox
    property alias removeButton: removeClick
    property alias showCheckBox: checkBoxHolder.visible
    property alias elementMouse: elementMouse

    color: "beige"
    // Change opacity when mouse is over the item
    opacity: elementMouse.containsMouse ? 0.5 : 1

    // MouseArea for handling clicks on the entire item
    MouseArea {
        id: elementMouse
        anchors.fill: parent
        onClicked: {
            // Handle item click here
        }
    }

    // Layout for arranging checkbox, task name, and remove button horizontally
    RowLayout {
        id: elementRow
        anchors.fill: parent
        spacing: 0

        // Checkbox section
        Rectangle {
            id: checkBoxHolder
            implicitWidth: parent.width * 0.2
            implicitHeight: parent.height
            color: "transparent"

            CheckBox {
                id: completionStatusBox
                width: parent.width / 2
                height: width
                checked: completionStatus
                checkable: true
                anchors.centerIn: parent
                onCheckedChanged: {
                    // Handle checkbox state change here
                }
            }
            Layout.fillWidth: true
        }

        // Task name section
        Rectangle {
            id: taskNameHolder
            implicitWidth: parent.width * 0.6
            implicitHeight: parent.height
            color: "transparent"

            Text {
                id: testNameText
                text: taskName
                anchors.fill: parent
                font {
                    family: "Roboto"
                    styleName: "Medium"
                    pixelSize: parent.height * 0.3
                    strikeout: completionStatus // Strike through if completed
                }
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        // Remove button section
        Rectangle {
            id: removeButton
            implicitWidth: parent.width * 0.2
            implicitHeight: parent.height
            color: "transparent"
            // Change opacity when mouse is over the remove button
            opacity: removeClick.containsMouse ? 0.5 : 1

            Image {
                id: removeButtonIcon
                source: "qrc:/icons/delete.png"
                anchors.centerIn: parent
                width: parent.height * 0.75
                height: width
                fillMode: Image.PreserveAspectFit
            }

            MouseArea {
                id: removeClick
                anchors.fill: parent
                propagateComposedEvents: false
                onClicked: {
                    // Handle remove button click here
                }
            }
            Layout.fillWidth: true
        }
    }
}
