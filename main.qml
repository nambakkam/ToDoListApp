import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

// Main application window
Window {
    width: 480
    height: 800
    visible: true
    title: qsTr("ToDo App")

    // Heading section with navigation buttons
    HeadingRect{
        id:headingRect
        anchors{
            top:parent.top
        }
        // Back button: visible only when not on the first page
        backButton{
            visible: pageSwitcher.currentIndex !== 0
            onClicked: {
                pageSwitcher.currentIndex = 0
            }
        }
        // Logs button: switches to logs page and refreshes logs
        logsButton{
            onClicked: {
                pageSwitcher.currentIndex = 2
                eventLogsModel.refresh()
            }
        }
    }

    // StackLayout for page navigation
    StackLayout{
        id:pageSwitcher
        width: parent.width
        height: parent.height - headingRect.height
        anchors.top: headingRect.bottom
        currentIndex: 0

        // First page: triggers navigation to second page on element click
        FirstPage{
            Layout.fillWidth: true
            Layout.fillHeight: true
            onElementClicked: {
                pageSwitcher.currentIndex = 1;
            }
        }

        // ToDo list view page
        ToDoListView{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        // Logs page
        LogsPage{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
