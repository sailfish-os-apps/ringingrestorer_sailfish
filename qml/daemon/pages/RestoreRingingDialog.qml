/**********************************************************************
 * Copyright 2015 Arto Jalkanen
 *
 * This file is part of RingingRestorer.
 *
 * RingingRestorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RingingRestorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RingingRestorer.  If not, see <http://www.gnu.org/licenses/>
**/

import QtQuick 2.0

import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: root

    function formatMinutes(minutes) {
        if (minutes <= 60) {
            return minutes + " minutes"
        }
        else {
            var hours = Math.floor(minutes / 60)
            var remainingMins = minutes % 60
            if (remainingMins === 0) {
                return hours + " hours"
            } else {
                return hours + "h" + remainingMins + "mins"
            }
        }
    }

    function restoreToMinutes(restore) {
        var mins = 0;
        if (restore <= 4) {
            return restore * 15
        } else if (restore <= 18) {
            // One hour + upto 7 hours
            return 60 + (restore - 4) * 30
        }
        // 8 hours++
        return 480 + (restore - 18) * 60
    }

    function restoreText() {
       var minutes = root.restoreToMinutes(restoreSlider.value)
       var time = backend.timeCurrentPlusMinutes(minutes)
       return "Restore ringing in " + root.formatMinutes(minutes) + ", at " + time
    }

    onAccepted: {
        console.log("onAccepted")
        // At least for now, no restoring of volume on Sailfish
        backend.restoreRingingIn(root.restoreToMinutes(restoreSlider.value), -1)
    }
    onRejected: {
        console.log("onRejected")
        backend.close();
    }
    onStatusChanged: {
        console.log("onStatusChanged", status)

        if (status === PageStatus.Active) {
            console.log("Starting cancel timer")
            cancelTimer.countDown = 15
            cancelTimer.running = true
            restoreSlider.refreshText()
        } else {
            console.log("Cancelling cancel timer")
            cancelTimer.running = false
        }
    }

    Column {
        anchors.fill: parent
        spacing: 150

        property int spaceBetweenSections: 40
        property int sliderMargins: 50

        DialogHeader {
            id: dialogHeader
            acceptText: "Accept"
            cancelText: "Cancel" + (cancelTimer.running ? " in " + cancelTimer.countDown + "s" : "");
        }

        Label {
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
            text: restoreSlider.label
        }

        Slider {
            id: restoreSlider
            label: restoreText() // value + " minutes"
            width: parent.width
            minimumValue: 1;
            maximumValue: 22;
            value: 1 // Initial value
            stepSize: 1
            onValueChanged: {
                if (cancelTimer !== null) cancelTimer.running = false
                refreshText()
                // restoreLabel.refreshText()
            }
            function refreshText() {
                label = restoreText()
            }
        }

        Timer {
            id: cancelTimer
            interval: 1000;
            repeat: true
            property int countDown

            onTriggered: {
                console.log("Cancel timer", countDown)
                countDown--
                if (countDown <= 0) {
                    cancelTimer.running = false;
                    rejected()
                }
            }
        }
    }
}


