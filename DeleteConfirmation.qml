import QtQuick 2.15
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.15

Popup {
    id: deleteConfirmPopup
    width: parent.width * 0.6
    height: parent.height * 0.3
    modal: true
    focus: true
    closePolicy: Popup.CloseOnPressOutside

    property int listIndex: 0        // To show which list is being deleted
    property string listName: ""
    signal confirmed()                  // Signal emitted when user confirms

    Rectangle {
        anchors.fill: parent
        color: "white"
        radius: width * 0.05
        border.color: "#cccccc"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: parent.width * 0.05
            spacing: parent.height * 0.05

            // Header
            Text {
                text: "Delete List?"
                font.pixelSize: parent.height * 0.12
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            // Message
            Text {
                text: "Are you sure you want to delete \"" + listName + "\"?"
                font.pixelSize: parent.height * 0.08
                wrapMode: Text.WordWrap
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
            }

            // Buttons Row
            RowLayout {
                Layout.fillWidth: true
                spacing: parent.width * 0.04

                Button {
                    text: "Yes"
                    Layout.fillWidth: true
                    height: parent.height * 0.2
                    onClicked: {
                        deleteConfirmPopup.confirmed()
                    }
                }

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
