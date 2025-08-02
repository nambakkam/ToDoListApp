import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
Window {
    width: 400
    height: 600
    visible: true
    title: qsTr("ToDo App")

    HeadingRect{
        id:headingRect
        anchors{
            top:parent.top
        }
        backButton{
            visible: pageSwitcher.currentIndex === 1
            onClicked: {
                pageSwitcher.currentIndex = 0
            }
        }
    }
    StackLayout{
        id:pageSwitcher
        width: parent.width
        height: parent.height - headingRect.height
        anchors.top: headingRect.bottom
        currentIndex: 0
        FirstPage{
            Layout.fillWidth: true
            Layout.fillHeight: true
            onElementClicked: {
                pageSwitcher.currentIndex = 1;
            }
        }
        ToDoListView{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }


}
