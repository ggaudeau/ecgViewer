import QtQuick
import QtQuick.Controls 2.15
import QtCharts 6.2
import Qt.labs.platform 1.1

import fr.ecgviewer.qmlcomponents 1.0

Window {
    id: mainWindow
    title: qsTr("ECG Viewer")

    width: 960
    height: 480
    visible: true

    Timer {
        id: startupPopup
        interval: 0
        running: true
        repeat: false

        onTriggered: {
            console.log("DEBUG: open pop-up")
            popup.open()
        }
    }

    Popup {
        id: popup
        width: loadSequenceButton.width
        height: loadSequenceButton.height
        anchors.centerIn: parent

        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

        Button {
            id: loadSequenceButton
            anchors.centerIn: parent
            text: "Load sequence"
            onClicked: {
                console.log("DEBUG: click on load sequence button")
                folderDialog.open()
            }
        }
    }

    function onLoadSequence(folder) {
        var url = Qt.url(folder);
        var path = url.toString().replace(/^(file:\/{2})/,"");

        console.log("DEBUG: select folder: " + path)
        if (ctrl.loadSequence(path)) {
            popup.close()
            column.visible = true

            ctrl.init( signalLineSeries, xValueAxis, yValueAxis )
        }
    }

    FolderDialog {
        id: folderDialog
        flags: FolderDialog.ShowDirsOnly | FolderDialog.ReadOnly
        onAccepted: function() { onLoadSequence(folderDialog.folder); }
        onRejected: console.log("DEBUG: reject folder dialog menu")
    }


    /**
      * This is the class that links the view (QML) with the logic code (C++).
      * Model is store in Sequence class agregated by the logic.
      **/

    Controller {
        id: ctrl
        windowWidthInSeconds: 6

        function reinit() {
            timer.stop()

            ctrl.init( signalLineSeries, xValueAxis, yValueAxis )
            playButton.text = "Play"
            progressBar.value = 0.0
        }
    }

    ///////

    Column {
        id: column
        visible: false

        Row {
            id: correctionPanel
            x: 0
            y: 0
            width: 300
            height: 30

            spacing: 2

            Label {
                id: averageWindowWidthLabel
                width: 100
                height: parent.height
                text: "Average window width"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                wrapMode: "WordWrap"
                font.pointSize: 10
            }

            TextEdit {
                id: averageWindowWidthValue
                x: averageWindowWidthLabel.right
                width: 50
                height: parent.height
                text: "30"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
            }

            Button {
                id: averageWindowWidthButton
                x: averageWindowWidthValue.right
                height: parent.height
                text: "Apply correction"
                onClicked: {
                    console.log("DEBUG: apply correction")
                    ctrl.applyCorrection( Number(averageWindowWidthValue.text) ) // TODO: handle error
                    ctrl.reinit();
                }                             
            }

            Button {
                id: averageWindowWidthResetButton
                x: averageWindowWidthButton.right
                height: parent.height
                text: "Reset correction"
                onClicked: {
                    console.log("DEBUG: reset correction")
                    ctrl.resetCorrection()
                    ctrl.reinit();
                }
            }

            Button {
                id: removeOutliersButton
                x: averageWindowWidthResetButton.right
                height: parent.height
                text: "Remove outliers"
                onClicked: {
                    console.log("DEBUG: remove outliers (destructive call)")
                    ctrl.removeOutliers()
                    ctrl.reinit();
                }
            }
        }

        ChartView {
            id: chartView
            width: mainWindow.width
            height: mainWindow.height - correctionPanel.height - playerPanel.height
            theme: ChartView.ChartThemeBrownSand
            antialiasing: true

            ValuesAxis {
                id: xValueAxis;
                min: 0
                max: 5
            }

            ValuesAxis {
                id: yValueAxis;
                min: 0
                max: 5
            }

            LineSeries {
                id: signalLineSeries
                name: "signal ECG"
                axisX: xValueAxis
                axisY: yValueAxis
                useOpenGL: true
            }

            Timer {
                id: timer
                interval: 50 // use 20 frames / sec
                running: false
                repeat: true

                onTriggered: {
                    if (ctrl.update( signalLineSeries, xValueAxis, yValueAxis )) {
                        progressBar.value = ctrl.getProgressValue();
                    } else {
                        console.log("DEBUG: interrupt timer")
                        ctrl.reinit()
                    }
                }
            }
        }

        Row {
            id: playerPanel

            spacing: 2
            x: 0
            y: chartView.bottom
            width: mainWindow.width
            height: 30
            padding: 0

            Button {
                id: playButton

                height: playerPanel.height
                text: "Play"
                onClicked: {
                    if (timer.running) {
                        // pause
                        timer.stop()
                        ctrl.pause()
                        text = "Resume"
                    } else {
                        if (text == "Resume") {
                            // resume
                            ctrl.resume()
                            timer.start()
                            text = "Pause"
                        } else {
                            // start
                            ctrl.play();
                            timer.start()
                            text = "Pause"
                        }
                    }
                }
            }

            Button {
                id: stopButton

                height: playerPanel.height
                text: "Stop"
                onClicked: {
                    ctrl.reinit()
                }
            }

            ProgressBar {
                id: progressBar

                width: playerPanel.width - playButton.width - stopButton.width
                height: playerPanel.height
                value: 0.0
            }
        }
    }
}
