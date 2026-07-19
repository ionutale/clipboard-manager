import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    required property string itemId
    required property string content
    required property string sourceApp
    required property string preview
    required property string timeAgo

    width: ListView.view.width
    height: 48

    Rectangle {
        anchors.fill: parent
        radius: 6
        color: mouseArea.containsMouse
            ? Qt.rgba(0, 0, 0, 0.05)
            : "transparent"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        spacing: 0

        Text {
            Layout.fillWidth: true
            text: root.preview
            font.pixelSize: 13
            color: Qt.rgba(0.11, 0.11, 0.12, 0.9)
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 2
            spacing: 6

            Text {
                text: root.timeAgo
                font.pixelSize: 11
                color: Qt.rgba(0, 0, 0, 0.4)
            }

            Rectangle {
                width: 3
                height: 3
                radius: 1.5
                color: Qt.rgba(0, 0, 0, 0.25)
                visible: root.sourceApp.length > 0
            }

            Text {
                text: root.sourceApp
                font.pixelSize: 11
                color: Qt.rgba(0, 0, 0, 0.3)
                elide: Text.ElideRight
                visible: root.sourceApp.length > 0
            }

            Item { Layout.fillWidth: true }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                ListView.view.model.copyItem(root.itemId)
                const list = ListView.view
                const timer = Qt.createQmlObject(
                    "import QtQuick; Timer { interval: 200; onTriggered: list.parent.hidePopup(); }",
                    root)
                timer.start()
            }
        }

        onPressAndHold: {
            contextMenu.open()
        }
    }

    Menu {
        id: contextMenu
        x: mouseArea.mouseX
        y: mouseArea.mouseY

        MenuItem {
            text: "Copy"
            onTriggered: {
                ListView.view.model.copyItem(root.itemId)
                root.ListView.view.parent.hidePopup()
            }
        }
        MenuItem {
            text: "Delete"
            onTriggered: ListView.view.model.deleteItem(root.itemId)
        }
    }
}
