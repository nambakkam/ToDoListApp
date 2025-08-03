import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Main container for the To-Do list view
Rectangle {
    id: tasksHolder
    width: parent.width
    height: parent.height
    color: "#f5f5f5"

    // Layout for organizing components vertically
    ColumnLayout {
        id: tasksColumn
        spacing: 10
        width: tasksHolder.width * 0.8
        height: tasksHolder.height * 0.8
        anchors.centerIn: parent

        // Scrollable area for displaying the list of tasks
        ScrollView {
            id: scrollArea
            Layout.fillWidth: true
            Layout.preferredHeight: parent.height * 0.6
            clip: true

            // Column to hold each task item
            Column {
                id: tasksList
                width: scrollArea.width
                spacing: 5

                // Dynamically creates task items from the model
                Repeater {
                    id: componentsRepeater
                    model: todoModel
                    delegate: ListDelegate {
                        width: tasksList.width
                        height: 60
                        serialNumber: model.id // Task serial number
                        taskName: model.ItemName // Task name
                        completionStatus: model.StatusRole // Completion status

                        // Handler for remove button click
                        removeButton.onClicked: {
                            deleteListConfirmation.listName = model.ItemName
                            deleteListConfirmation.listIndex = index
                            deleteListConfirmation.open()
                        }

                        // Handler for checkbox state change
                        checkBoxComp.onCheckedChanged: {
                            todoModel.toggleTaskStatus(index,checkBoxComp.checked)
                        }
                    }
                }
            }
        }

        // Row containing input field and add button
        RowLayout {
            id: inputRow
            spacing: 10
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            // Text field for entering new tasks
            TextField {
                id: newTaskField
                Layout.fillWidth: true
                placeholderText: "Enter new task"
                Keys.onReturnPressed: addButton.clicked() // Add task on Enter key
            }

            // Button to add new task to the list
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

    // Confirmation dialog for deleting a task
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
