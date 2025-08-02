import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

Popup {
    id: createNewListPopup
    width: parent.width * 0.75
    height: parent.height * 0.4
    closePolicy: Popup.CloseOnPressOutside
    modal: true
    focus: true
    property alias saveButtonAlias: saveButton
    property alias listNameField: listNameInput

    Rectangle {
        anchors.fill: parent
        color: "white"
        radius: width * 0.02
        border.color: "#cccccc"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: parent.width * 0.05
            spacing: parent.height * 0.05

            // Header
            Text {
                text: qsTr("Create New List")
                font.pixelSize: parent.height * 0.08
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // Text input field
            TextField {
                id: listNameInput
                placeholderText: "Enter list name"
                font.pixelSize: parent.height * 0.06
                height: parent.height * 0.3
                Layout.fillWidth: true
                padding: parent.width * 0.02
                background: Rectangle {
                    radius: width * 0.1
                    border.color: "#cccccc"
                    color: "#f9f9f9"
                }
            }

            // Buttons Row
            RowLayout {
                Layout.fillWidth: true
                spacing: parent.width * 0.04

                Button {
                    id:saveButton
                    text: "Save"
                    Layout.fillWidth: true
                    height: parent.height * 0.15
                    onClicked: {
                        console.log("List Name:", listNameInput.text)
                    }
                }

                Button {
                    id:cancelButton
                    text: "Cancel"
                    Layout.fillWidth: true
                    height: parent.height * 0.15
                    onClicked: createNewListPopup.close()
                }
            }
        }
    }
    onClosed: {
        listNameInput.text = ""
    }
    onOpened: {
        listNameInput.forceActiveFocus()
    }
}
