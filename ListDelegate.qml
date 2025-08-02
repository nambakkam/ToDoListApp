import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.0
Rectangle{
    width: parent.width
    height: width*0.15
    property string serialNumber: "1"
    property string taskName: "taskName"
    property bool completionStatus: false
    property alias checkBoxComp: completionStatusBox
    property alias removeButton: removeClick
    property alias showCheckBox: checkBoxHolder.visible
    property alias elementMouse: elementMouse
    color: "beige"
    opacity: elementMouse.containsMouse? 0.5:1
    MouseArea{
        id:elementMouse
        anchors.fill: parent
        onClicked: {

        }
    }
    RowLayout {
        id:elementRow
        anchors.fill: parent
        spacing: 0
        Rectangle{
            id:checkBoxHolder
            implicitWidth: parent.width*0.2
            implicitHeight: parent.height
            color: "transparent"
            CheckBox{
                id:completionStatusBox
                width: parent.width/2
                height: width
                checked: completionStatus
                checkable: true
                anchors.centerIn: parent
                onCheckedChanged: {
                }
            }
            Layout.fillWidth: true
        }
        Rectangle{
            id:taskNameHolder
            implicitWidth: parent.width*0.6
            implicitHeight: parent.height
            color: "transparent"
            Text {
                id: testNameText
                text: taskName
                anchors.fill: parent
                font{
                    family: "Roboto"
                    styleName: "Medium"
                    pixelSize: parent.height*0.3
                    strikeout: completionStatus
                }
                wrapMode: Text.WrapAnywhere
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle{
            id:removeButton
            implicitWidth: parent.width*0.2
            implicitHeight: parent.height
            color: "transparent"
            opacity: removeClick.containsMouse ? 0.5:1
            Image {
                id: removeButtonIcon
                source: "qrc:/icons/delete.png"
                anchors.centerIn: parent
                width: parent.height*0.75
                height: width
                fillMode: Image.PreserveAspectFit
            }
            MouseArea{
                id:removeClick
                anchors.fill: parent
                propagateComposedEvents: false
                onClicked: {

                }
            }
            Layout.fillWidth: true
        }
    }

}


