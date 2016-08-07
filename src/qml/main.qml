import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import org.kde.plasma.core 2.0 as PlasmaCore
import QtWebKit 3.0

import org.plasma_light.kcm 1.0

Rectangle {
    width: 824
    height: 530

    color: "#EFF0F1"

    Kernels {
        id: kernelsModel

        Component.onCompleted: {
            kernelsModel.fetchData()
        }

        onJobStarted: {
            progressDialogMsg.text = message
            progressDialog.visible = true
        }

        onJobUpdated: {
            if (message != "")
                progressDialogMsg.text = message

            if (progress > 100)
                progressDialogBar.indeterminate = true
            else {
                progressDialogBar.indeterminate = false
                progressDialogBar.value = progress
            }
            progressDialog.visible = true
        }

        onJobFinished: {
            progressDialog.visible = false

            if (!succeed) {
                errorDialog.visible = true
                errorDialogMsg.text = message
            }
        }

        onSystemDataChanged: {
            kernelsModel.fetchData()
        }
    }

    Text {
        id: header
        text: i18n("Install or remove kernels from your system")
        font.pointSize: 16

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 30
        anchors.bottomMargin: 19
        anchors.leftMargin: 12
        anchors.rightMargin: 8
    }

    Rectangle {
        id: kernelsList
        color: "transparent"

        height: 202

        border.width: 1
        border.color: "#BABCBE"
        radius: 3

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: globalPrimaryButtons.top
        anchors.leftMargin: 12
        anchors.rightMargin: 8
        anchors.topMargin: 12
        anchors.bottomMargin: 12

        Rectangle {
            id: listViewContainer
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: actionButtons.top

            anchors.leftMargin: 8
            anchors.rightMargin: 8
            anchors.topMargin: 11
            anchors.bottomMargin: 11

            border.width: 1
            border.color: "#BABCBE"

            ScrollView {
                anchors.fill: parent

                ListView {
                    id: list

                    highlight: highlight
                    highlightFollowsCurrentItem: true

                    // spacing: 16
                    model: kernelsModel
                    delegate: itemDelegate

                    header: Rectangle {
                        width: listViewContainer.width
                        height: 26
                        color: "lightgray"
                        z: 10

                        border.color: "#BABCBE"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            Text {
                                text: i18n("Kernel Version")
                                Layout.minimumWidth: 200
                                Layout.maximumWidth: 200
                            }
                            Text {
                                Layout.minimumWidth: 100
                                Layout.maximumWidth: 100
                                text: i18n("Status")
                            }
                            Text {
                                Layout.minimumWidth: 100
                                Layout.maximumWidth: 100
                                text: i18n("Support")
                            }
                            Text {
                                Layout.minimumWidth: 50
                                Layout.maximumWidth: 50
                                text: i18n("Active")
                            }
                            Text {
                                Layout.minimumWidth: 50
                                Layout.maximumWidth: 50
                                text: i18n("Default")
                            }
                            Text {
                                Layout.fillWidth: true
                                text: i18n("Upgrade available")
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            id: actionButtons
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            anchors.margins: 8

            spacing: 12

            Button {
                text: i18n("Changelog")
                enabled: list.currentItem
                onClicked: {
                    var changeLogUrl = kernelsModel.getChangeLogUrl(
                                list.currentIndex)
                    // print("ChangeLog URL ", changeLogUrl)
                    changeLogDialogWebView.url = changeLogUrl
                    changeLogDialog.visible = true
                }
            }

            Button {
                enabled: list.currentItem
                         && list.currentItem.dataModel.isUpgradable
                text: i18n("Update")
                onClicked: {
                    if (list.currentIndex < 0)
                        return

                    kernelsModel.update(list.currentIndex)
                }
            }
            Button {
                text: i18n("Set as Default")
                enabled: list.currentItem && list.currentItem.dataModel.isInstalled && !list.currentItem.dataModel.isDefault? true : false
                onClicked: {
                    if (list.currentIndex < 0)
                        return

                    kernelsModel.setAsDefault(list.currentIndex)
                }
            }

            Button {
                text: list.currentItem
                      && list.currentItem.dataModel.isInstalled ? i18n("Remove") : i18n(
                                                                      "Install")
                enabled: list.currentItem ? true : false
                onClicked: {
                    if (list.currentIndex < 0)
                        return

                    if (list.currentItem.dataModel.isInstalled)
                        kernelsModel.remove(list.currentIndex)
                    else
                        kernelsModel.install(list.currentIndex)
                }
            }
        }
    }

    RowLayout {
        id: globalPrimaryButtons
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        anchors.margins: 8

        spacing: 12

        Button {
            text: i18n("Apply")
            onClicked: {

            }
        }
    }

    RowLayout {
        id: globalSecondaryButtons
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        anchors.margins: 8

        spacing: 12

        Button {
            text: i18n("Help")
            onClicked: {

            }
        }

        Button {
            text: i18n("Reset")
            enabled: false
            onClicked: {

            }
        }
    }

    Component {
        id: itemDelegate
        Rectangle {
            id: delegateRootItem

            color: "transparent"
            width: listViewContainer.width
            height: 20


            property var dataModel
            Component.onCompleted: {
                delegateRootItem.dataModel = model
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 16

                Text {
                    text: name
                    Layout.minimumWidth: 200
                    Layout.maximumWidth: 200
                    Layout.fillWidth: true
                }

                Text {
                    text: isInstalled ? i18n("Installed") : i18n("Not Installed")
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 100
                }

                Text {
                    text: support
                    Layout.minimumWidth: 100
                    Layout.maximumWidth: 100
                }

                Text {
                    text: isActive ? i18n("Yes") : i18n("No")
                    Layout.minimumWidth: 50
                    Layout.maximumWidth: 50
                }

                Text {
                    text: isDefault ? i18n("Yes") : i18n("No")
                    Layout.minimumWidth: 50
                    Layout.maximumWidth: 50
                }

                Text {
                    text: isUpgradable ? i18n("Yes") : i18n("No")
                    Layout.minimumWidth: 50
                    Layout.maximumWidth: 50
                    Layout.fillWidth: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: list.currentIndex = index
                }
            }
        }
    }

    Component {
        id: highlight
        Rectangle {
            color: "lightsteelblue"
            radius: 0
        }
    }

    Dialog {
        id: changeLogDialog
        visible: false
        modality: Qt.ApplicationModal

        title: i18n("ChangeLog")
        contentItem: Rectangle {
            implicitHeight: 600
            implicitWidth: 800

            color: "#EFF0F1"

            WebView {
                id: changeLogDialogWebView
                anchors.fill: parent
            }
        }
    }

    Dialog {
        id: progressDialog
        visible: false
        modality: Qt.ApplicationModal

        title: i18n("Executing task")
        contentItem: Rectangle {
            implicitHeight: 100
            implicitWidth: 400

            color: "#EFF0F1"

            Text {
                id: progressDialogMsg
                text: i18n("Applying changes, please waith.")
                anchors.bottom: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ProgressBar {
                id: progressDialogBar
                anchors.top: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                minimumValue: 0
                maximumValue: 100
                indeterminate: true
            }
        }
    }

    Dialog {
        id: errorDialog
        visible: false
        modality: Qt.ApplicationModal

        standardButtons: StandardButton.Ok

        title: i18n("Unable to complete the task")
        contentItem: Rectangle {
            implicitHeight: 100
            implicitWidth: 400

            color: "#EFF0F1"

            Text {
                id: errorDialogMsg
                text: i18n("There was an error, that's all I know.")
                anchors.centerIn: parent
            }
        }
    }
}
