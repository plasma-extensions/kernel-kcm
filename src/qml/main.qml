/*
 * My project
        */
import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0

import org.kde.plasma.core 2.0 as PlasmaCore
import QtWebKit 3.0

import org.plasma_light.kcm 1.0

Rectangle {
    width: 824
    height: 530

    color: "#EFF0F1"

    Kernels {
        id: kernelsModel
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
                anchors.leftMargin: 16
                anchors.topMargin: 11

                ListView {
                    id: list

                    highlight: highlight
                    highlightFollowsCurrentItem: true
                    spacing: 16

                    model: kernelsModel
                    delegate: itemDelegate

                    header: Rectangle {
                        width: parent.width
                        height: childrenRect.height
                        color: "white"
                        z: 100

                        RowLayout {
                            spacing: 10
                            Text {
                                text: i18n("Kernel Version")
                            }
                            Text {
                                text: i18n("Status")
                            }
                            Text {
                                text: i18n("Support")
                            }
                            Text {
                                text: i18n("Active")
                            }
                            Text {
                                text: i18n("Default")
                            }
                            Text {
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
                onClicked: {

                }
            }

            Button {
                text: i18n("Update")
                onClicked: {
                }
            }
            Button {
                text: list.currentItem.dataModel.isInstalled ? i18n("Remove") :i18n("Install")
                onClicked: {
                    if (list.currentItem.dataModel.isInstalled)
                        kernelsModel.removeKernel(list.currentIndex);
                    else
                        kernelsModel.installKernel(list.currentIndex);
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
        RowLayout {
            id: delegateRootItem
            property var dataModel;
            Component.onCompleted: {
                delegateRootItem.dataModel = model;
            }
            Text {
                text: name
            }

            Text {
                text: isInstalled ? i18n("Installed") : i18n("Not Installed")
            }

            Text {
                text: isLts ? i18n("LTS") : i18n("Not LTS")
            }

            Text {
                text: isActive ? i18n("Yes") : i18n("No")
            }

            Text {
                text: isDefault ? i18n("Yes") : i18n("No")
            }

            Text {
                text: isUpgradable ? i18n("Yes") : i18n("No")
                Layout.fillWidth: true
            }

            MouseArea {
                anchors.fill: parent
                onClicked: list.currentIndex = index
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
}
