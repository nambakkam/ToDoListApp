import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: tasksHolder
    width: parent.width
    height: parent.height
    color: "#f5f5f5"

    ColumnLayout {
        id: tasksColumn
        spacing: 10
        width: tasksHolder.width * 0.8
        height: tasksHolder.height * 0.8
        anchors.centerIn: parent

        // Scrollable task list
        ScrollView {
            id: scrollArea
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.6
            clip: true

            Column {
                id: tasksList
                width: scrollArea.width
                spacing: 5

                Repeater {
                    id: componentsRepeater
                    model: todoModel
                    delegate: ListDelegate {
                        width: tasksList.width
                        height: 60
                        serialNumber: model.id
                        taskName: model.ItemName
                        completionStatus: model.StatusRole
                        removeButton.onClicked: {
                            deleteListConfirmation.listName = model.ItemName
                            deleteListConfirmation.listIndex = index
                            deleteListConfirmation.open()
                        }
                        checkBoxComp.onCheckedChanged: {
                            todoModel.toggleTaskStatus(index,checkBoxComp.checked)
                        }
                    }
                }
            }
        }

        // Row for TextField + Add Button
        RowLayout {
            id: inputRow
            spacing: 10
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            TextField {
                id: newTaskField
                Layout.fillWidth: true
                placeholderText: "Enter new task"
                Keys.onReturnPressed: addButton.clicked() // Add on Enter press
            }

            Button {
                id: addButton
                text: "Add"
                onClicked: {
                    if (newTaskField.text.trim() !== "") {
                        todoModel.addItemToList(newTaskField.text)
                        newTaskField.text = ""
                    }
                }
            }
        }
    }
    DeleteConfirmation{
        id:deleteListConfirmation
        width: parent.width*0.75
        height: parent.height*0.5
        anchors.centerIn: parent
        onConfirmed: {
            console.log("The index is ", listIndex)
            todoModel.removeItemFromList(deleteListConfirmation.listIndex)
            deleteListConfirmation.close()
        }
    }
}
