/**
 * @file ProfileConfirmPopup.qml
 * @brief Modal popup asking for the name of a new list.
 *
 * Despite the file name it has nothing to do with profiles. The Save button's
 * default handler only logs the text; the owner (FirstPage) overrides it via
 * `saveButtonAlias`. The field is cleared on close and focused on open.
 */
import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

// Popup for creating a new list
Popup {
    id: createNewListPopup
    width: parent.width * 0.75
    height: parent.height * 0.4
    closePolicy: Popup.CloseOnPressOutside // Close when clicking outside
    modal: true
    focus: true

    /** The Save button (attach `onClicked` to act on the entered name). */
    property alias saveButtonAlias: saveButton
    /** The name TextField (read `text`). */
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

            // Header text
            Text {
                text: qsTr("Create New List")
                font.pixelSize: parent.height * 0.08
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // Input field for list name
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

            // Row for Save and Cancel buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: parent.width * 0.04

                // Save button
                Button {
                    id:saveButton
                    text: "Save"
                    Layout.fillWidth: true
                    height: parent.height * 0.15
                    onClicked: {
                        // Log the entered list name
                        console.log("List Name:", listNameInput.text)
                    }
                }

                // Cancel button
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

    // Clear input when popup is closed
    onClosed: {
        listNameInput.text = ""
    }

    // Focus input when popup is opened
    onOpened: {
        listNameInput.forceActiveFocus()
    }
}
