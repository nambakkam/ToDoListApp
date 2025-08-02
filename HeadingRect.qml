import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: headingRect
    width: parent.width
    height: parent.height * 0.1
    color: "#7ADAA5"
    property alias backButton: backButton
    property alias logsButton: logsButton
    RowLayout {
        id: headerLayout
        anchors.fill: parent
        anchors.leftMargin: 0.03*parent.width
        anchors.rightMargin: 0.03*parent.width
        spacing: parent.width * 0.06

        // Back Button
        Button {
            id: backButton
            visible: false        // You can toggle this visibility dynamically
            text: "\u25C0"        // Unicode for a simple left arrow (or use an icon)
            Layout.preferredWidth: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                console.log("Back button clicked")
            }
        }

        // Todo Image
        Image {
            id: todoImage
            source: "qrc:/icons/check-list.png"
            Layout.preferredHeight: parent.height * 0.75
            Layout.preferredWidth: height
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter
        }

        // Heading Text
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
        Button {
            id: logsButton
            visible: true        // You can toggle this visibility dynamically
            text: "logs"        // Unicode for a simple left arrow (or use an icon)
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                console.log("logs button clicked")
            }
        }
    }
}
