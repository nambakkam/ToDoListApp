import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id:firstPage
    width: parent.width
    height: parent.height
    signal elementClicked();
    ListModel{
        id:notesList
        ListElement{ name:"C++ CheckList" }
        ListElement{ name:"Java CheckList" }
        ListElement{ name:"Git Essentials" }
    }

    ColumnLayout{
        id:notesColumn
        spacing: 5
        width: firstPage.width * 0.8
        height: firstPage.height * 0.6
        anchors.centerIn: parent

        // Create new list button
        Rectangle{
            id:createNewList
            implicitWidth: parent.width
            implicitHeight: parent.height*0.15
            color: "beige"
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            border.width: 2
            border.color:"#7ADAA5"
            radius: height*0.1
            opacity: newListMouse.containsMouse ? 0.5 : 1
            Text {
                id: createText
                text: qsTr("Create New List")
                anchors.fill: parent
                font{
                    family: "Roboto"
                    styleName: "Medium"
                    pixelSize: parent.height*0.45
                }
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            MouseArea{
                id:newListMouse
                anchors.fill: parent
                onClicked: {
                    nameConfirmationPopup.open()
                }
            }
        }

        // Make this part scrollable
        ScrollView {
            id: scrollArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Column {
                id: notesContainer
                width: parent.width
                spacing: 5

                Repeater {
                    id: repeatNotes
                    model: notesList
                    Layout.fillWidth: true
                    delegate: ListDelegate {
                        implicitWidth: notesContainer.width
                        implicitHeight: 0.2 * width
                        taskName: model.name
                        showCheckBox: false
                        removeButton.onClicked: {
                            deleteListConfirmation.listIndex = index
                            deleteListConfirmation.listName = model.name
                            deleteListConfirmation.open()
                        }
                        elementMouse.onClicked: {
                            elementClicked()
                        }
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    ProfileConfirmPopup{
        id:nameConfirmationPopup
        width: parent.width*0.75
        height: parent.height*0.5
        anchors.centerIn: parent
        saveButtonAlias.onClicked: {
            if (listNameField.text.trim() !== "") {
                notesList.append({ "name": listNameField.text })
            }
            nameConfirmationPopup.close()
        }
    }
    DeleteConfirmation{
        id:deleteListConfirmation
        width: parent.width*0.75
        height: parent.height*0.5
        anchors.centerIn: parent
        onConfirmed: {
            notesList.remove(deleteListConfirmation.listIndex)
            deleteListConfirmation.close()
        }
    }
}
