import QtQuick
import QtQuick.Controls

Item {
    id: root

    signal searchTextChanged(string text)
    signal escapePressed()
    signal downPressed()

    property alias text: input.text

    implicitHeight: 36

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: Qt.rgba(0, 0, 0, 0.04)
    }

    TextInput {
        id: input
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12
        verticalAlignment: TextInput.AlignVCenter
        font.pixelSize: 13
        color: "#1d1d1f"
        cursorVisible: activeFocus

        onTextChanged: root.searchTextChanged(input.text)

        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Escape) {
                root.escapePressed()
                event.accepted = true
            } else if (event.key === Qt.Key_Down) {
                root.downPressed()
                event.accepted = true
            }
        }
    }
}
