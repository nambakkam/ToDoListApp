import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

// Main heading rectangle for the app header
Rectangle {
    id: headingRect
    width: parent.width
    height: parent.height * 0.1
    color: "#7ADAA5"

    // Expose buttons as properties for external access
    property alias backButton: backButton
    property alias logsButton: logsButton

    // Layout for header contents
    RowLayout {
        id: headerLayout
        anchors.fill: parent
        anchors.leftMargin: 0.03*parent.width
        anchors.rightMargin: 0.03*parent.width
        spacing: parent.width * 0.06

        // Back Button (hidden by default, can be shown dynamically)
        Button {
            id: backButton
            visible: false        // Toggle visibility as needed
            text: "\u25C0"        // Unicode left arrow
            Layout.preferredWidth: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                console.log("Back button clicked")
            }
        }

        // Todo Image (app icon)
        Image {
            id: todoImage
            source: "qrc:/icons/check-list.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: height
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter
        }

        // Heading Text ("TODO App")
        Text {
            id: todoAppHeading
            text: qsTr("TODO App")
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            font {
                family: "Roboto"
                styleName: "Medium"
                pixelSize: parent.height * 0.2
                bold: true
            }
            horizontalAlignment: Text.AlignLeft
        }

        // Logs Button (visible by default)
        Button {
            id: logsButton
            visible: true        // Toggle visibility as needed
            text: "Logs"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                console.log("logs button clicked")
            }
        }
    }
}
