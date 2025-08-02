import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ListView {
    id: eventLogsView
//    anchors.fill: parent
    model: eventLogsModel
    clip: true
    delegate: Rectangle {
//        implicitWidth: parent.width
        implicitHeight: 60
        color: index % 2 === 0 ? "#f0f0f0" : "#ffffff"

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20

            Text { text: eventType; width: parent.width * 0.2; font.bold: true; Layout.fillWidth: true }
            Text { text: noteName; width: parent.width * 0.3; Layout.fillWidth: true }
            Text { text: taskName; width: parent.width * 0.3; Layout.fillWidth: true }
            Text { text: timestamp; width: parent.width * 0.2; Layout.fillWidth: true }
        }
    }
}
