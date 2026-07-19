import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: root

    width: 400
    height: compactHeight

    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    property int compactHeight: 80
    property int expandedHeight: 480
    property bool expanded: false

    visible: false

    onActiveChanged: {
        if (!active) hidePopup()
    }

    function showPopup() {
        positionPopup()
        expanded = false
        height = compactHeight
        opacity = 0
        visible = true
        openAnim.restart()
        listView.model.refresh()
        searchField.forceActiveFocus()
    }

    function hidePopup() {
        closeAnim.restart()
    }

    function toggle() {
        if (visible) hidePopup()
        else showPopup()
    }

    function positionPopup() {
        const screen = Screen.primaryScreen
        if (!screen) return
        root.setX(screen.width - root.width - 20)
        root.setY(40)
    }

    NumberAnimation on opacity {
        id: openAnim
        from: 0
        to: 1
        duration: 120
        easing.type: Easing.OutCubic
    }

    SequentialAnimation {
        id: closeAnim
        NumberAnimation {
            target: root
            property: "opacity"
            to: 0
            duration: 80
            easing.type: Easing.InCubic
        }
        PropertyAction { target: root; property: "visible"; value: false }
    }

    SpringAnimation {
        id: expandAnim
        target: root
        property: "height"
        from: compactHeight
        to: expandedHeight
        spring: 3
        damping: 0.25
    }

    SpringAnimation {
        id: collapseAnim
        target: root
        property: "height"
        from: expandedHeight
        to: compactHeight
        spring: 3
        damping: 0.3
    }

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: Qt.rgba(0.96, 0.96, 0.97, 0.92)
        border.color: Qt.rgba(0, 0, 0, 0.06)
        border.width: 1

        Item {
            anchors.fill: parent
            anchors.margins: 12

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ClipboardSearchField {
                    id: searchField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36

                    onSearchTextChanged: function(text) {
                        listView.model.search(text)
                        if (text.length > 0 && !root.expanded) {
                            root.expanded = true
                            expandAnim.restart()
                        } else if (text.length === 0 && root.expanded) {
                            root.expanded = false
                            collapseAnim.restart()
                        }
                    }

                    onEscapePressed: root.hidePopup()
                    onDownPressed: listView.forceActiveFocus()
                }

                ListView {
                    id: listView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 4

                    model: clipboardModel
                    delegate: ClipboardItemDelegate { }
                    clip: true

                    spacing: 2

                    Keys.onEscapePressed: root.hidePopup()
                    Keys.onReturnPressed: {
                        if (currentIndex >= 0) {
                            const item = model.get(currentIndex)
                            listView.model.copyItem(item.itemId)
                            flashTimer.restart()
                        }
                    }

                    Timer {
                        id: flashTimer
                        interval: 200
                        onTriggered: root.hidePopup()
                    }

                    ScrollBar.vertical: ScrollBar {
                        active: listView.movingVertically
                        width: 4
                        policy: ScrollBar.AsNeeded
                    }
                }
            }
        }
    }
}
