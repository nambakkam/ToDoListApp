import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

// Popup dialog for confirming deletion of a list
Popup {
    id: deleteConfirmPopup
    width: parent.width * 0.6
    height: parent.height * 0.3
    modal: true
    focus: true
    closePolicy: Popup.CloseOnPressOutside

    // Properties to identify the list being deleted
    property int listIndex: 0        // Index of the list to delete
    property string listName: ""     // Name of the list to delete
    signal confirmed()               // Emitted when user confirms deletion

    Rectangle {
        anchors.fill: parent
        color: "white"
        radius: width * 0.05
        border.color: "#cccccc"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: parent.width * 0.05
            spacing: parent.height * 0.05

            // Header text
            Text {
                text: "Delete List?"
                font.pixelSize: parent.height * 0.12
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // Confirmation message with list name
            Text {
                text: "Are you sure you want to delete \"" + listName + "\"?"
                font.pixelSize: parent.height * 0.08
                wrapMode: Text.WordWrap
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
            }

            // Row of Yes/No buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: parent.width * 0.04

                // Yes button: emits confirmed signal
                Button {
                    text: "Yes"
                    Layout.fillWidth: true
                    height: parent.height * 0.2
                    onClicked: {
                        deleteConfirmPopup.confirmed()
                    }
                }

                // No button: closes the popup
                Button {
                    text: "No"
                    Layout.fillWidth: true
                    height: parent.height * 0.2
                    onClicked: deleteConfirmPopup.close()
                }
            }
        }
    }
}
