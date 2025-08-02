import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.0
RowLayout {
    id:elementRow
    width: parent.width
    height: width*0.15
    property string serialNumber: "1"
    property string taskName: "taskName"
    property bool completionStatus: false
    property alias checkBoxComp: completionStatusBox
    property alias removeButton: removeClick
    spacing: 0
//    Layout.fillWidth: true
//    Layout.fillHeight: true
    Text {
        id: idText
        text:serialNumber
        width: parent.width*0.2
        height: parent.height
        font{
            family: "Roboto"
            styleName: "Medium"
            pixelSize: parent.height*0.7
        }
        wrapMode: Text.WrapAnywhere
        Layout.fillWidth: true
    }
    Text {
        id: testNameText
        text: taskName
        width: parent.width*0.4
        height: parent.height
        font{
            family: "Roboto"
            styleName: "Medium"
            pixelSize: parent.height*0.7
        }
        wrapMode: Text.WrapAnywhere
        Layout.fillWidth: true
    }
    Rectangle{
        id:checkBoxHolder
        implicitWidth: parent.width*0.2
        implicitHeight: parent.height
        CheckBox{
            id:completionStatusBox
            width: parent.width/2
            height: width
            checked: completionStatus
            checkable: true
            anchors.centerIn: parent
        }
        Layout.fillWidth: true
    }
    Rectangle{
        id:removeButton
        implicitWidth: parent.width*0.2
        implicitHeight: parent.height
        Image {
            id: removeButtonIcon
            source: "qrc:/icons/delete.png"
            anchors.centerIn: parent
            width: parent.height
            height: width
            fillMode: Image.PreserveAspectFit
        }
        MouseArea{
            id:removeClick
            onClicked: {

            }
        }
        Layout.fillWidth: true
    }
}
