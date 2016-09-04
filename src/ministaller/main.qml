/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4

Window {
    visible: true
    height: 350
    width: 500

    function scrollToBottom() {
        var flickable = scrollView.flickableItem
        if (flickable.contentHeight > flickable.height) {
            flickable.contentY = flickable.contentHeight - flickable.height
        } else {
            flickable.contentY = 0
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        ScrollView {
            id: scrollView
            anchors.left: parent.left
            anchors.right: parent.right
            Layout.fillHeight: true

            TextEdit {
                readOnly: true
                text: liveLog.contents
                onTextChanged: scrollToBottom()
            }
        }

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Close")
            }
        }
    }
}
