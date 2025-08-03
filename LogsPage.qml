import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// ListView to display event logs
ListView {
    id: eventLogsView
    // anchors.fill: parent // Uncomment to fill parent container
    model: eventLogsModel // Model providing log data
    clip: true // Ensures content is clipped to view bounds

    // Delegate defines how each log entry is displayed
    delegate: Rectangle {
        // implicitWidth: parent.width // Uncomment to set width
        implicitHeight: 60 // Height for each log entry
        color: index % 2 === 0 ? "#f0f0f0" : "#d9bcbcff" // Alternating row colors

        // Layout for log entry fields
        RowLayout {
            anchors.verticalCenter: parent.verticalCenter // Center vertically
            spacing: 20 // Space between fields

            // Display event type
            Text { 
                text: eventType
                width: parent.width * 0.2
                font.bold: true
                Layout.fillWidth: true
            }
            // Display note name
            Text { 
                text: noteName
                width: parent.width * 0.3
                Layout.fillWidth: true
            }
            // Display task name
            Text { 
                text: taskName
                width: parent.width * 0.3
                Layout.fillWidth: true
            }
            // Display timestamp
            Text { 
                text: timestamp
                width: parent.width * 0.2
                Layout.fillWidth: true
            }
        }
    }
}
