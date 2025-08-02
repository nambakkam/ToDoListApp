import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
Rectangle {
    id:tasksHolder
    width: parent.width
    height: parent.height


    ColumnLayout{
        id:tasksColumn
        spacing: 0
        width: tasksHolder.width * 0.8
        height: tasksHolder.height * 0.6
        anchors.centerIn: parent

        Repeater{
            id:componentsRepeater
            width: parent.width
            height: parent.height
            model: todoModel
            delegate:ListDelegate{
                implicitWidth: parent.width
                implicitHeight: 0.2*width
                serialNumber: model.id
                taskName: model.ItemName
                completionStatus: model.StatusRole
                Layout.fillWidth: true
                Component.onCompleted: {
                    console.log(model.id,model.ItemName,model.StatusRole)
                }
            }

        }

    }

}
