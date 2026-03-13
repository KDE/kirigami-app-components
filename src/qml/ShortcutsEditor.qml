// SPDX-FileCopyrightText: 2024 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC

Kirigami.ScrollablePage {
    id: root

    property alias model: searchFilterProxyModel.sourceModel

    title: i18ndc("kirigami-actioncollection", "@title:window", "Shortcuts")

    actions: Kirigami.Action {
        displayComponent: Kirigami.SearchField {
            placeholderText: i18ndc("kirigami-actioncollection", "@label:textbox", "Filter…")
            onTextChanged: searchFilterProxyModel.setFilterFixedString(text);
        }
    }

    ListView {
        id: listView

        currentIndex: -1

        model: KSortFilterProxyModel {
            id: searchFilterProxyModel
            sourceModel: AC.ActionModel {
                id: actionModel
                // TODO: this should just fetch all actions of all collections
                collectionName: "org.kde.collection"
                shownActions: ActionModel.ActiveActions
            }

            filterRoleName: 'actionName'
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        section {
            property: "actionCollection"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                required property string section
                text: section
                width: ListView.view.width
            }
        }

        delegate: QQC.ItemDelegate {
            id: shortcutDelegate

            required property int index
            // Bind model in order to have editing work
            required property var model

            width: ListView.view.width
            text: model.actionDescription.text

            Accessible.description: model.actionDescription.shortcut

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: implicitWidth
                    source: model.actionDescription.icon.name || model.actionDescription.icon.source
                }

                QQC.Label {
                    text: model.actionDescription.text
                    Layout.fillWidth: true
                    Accessible.ignored: true
                }

                QQC.Label {
                    id: shortcutLabel
                    text: model.shortcut
                    Accessible.ignored: true
                }
                Kirigami.Separator {
                    visible: alternateShortcutLabel.text.length > 0 && shortcutLabel.text.length > 0
                    Layout.fillHeight: true
                }
                QQC.Label {
                    id: alternateShortcutLabel
                    visible: text.length > 0
                    text: model.alternateShortcut
                    Accessible.ignored: true
                }
            }

            onClicked: {
                shortcutDialog.title = i18ndc("kirigami-actioncollection", "@title:window", "Shortcut: %1",  model.actionDescription.text);
                shortcutDialog.actionDescription = model.actionDescription;
                shortcutDialog.keySequence = model.shortcut;
                shortcutDialog.alternateKeySequence = model.alternateShortcut;
                shortcutDialog.index = shortcutDelegate.index;
                shortcutDialog.shortcutDelegate = shortcutDelegate
                shortcutDialog.open()
            }
        }

        QQC.Dialog {
            id: shortcutDialog

            width: Math.max(implicitWidth, Kirigami.Units.gridUnit * 20)
            property ActionData actionDescription
            property alias keySequence: keySequenceItem.keySequence
            property alias alternateKeySequence: alternateKeySequenceItem.keySequence
            // Needs to access shortcutDelegate.model for doing setData on the model
            property Item shortcutDelegate
            property int index: -1
            modal: true

            parent: root.QQC.Overlay.overlay

            contentItem: ColumnLayout {
                spacing: 0
                KeySequenceItem {
                    id: keySequenceItem
                    Layout.fillWidth: true

                    defaultKeySequence: shortcutDialog.actionDescription?.defaultShortcut ?? ""

                    label: i18ndc("kirigami-actioncollection", "@label", "Shortcut:")
                    onKeySequenceModified: {
                        shortcutDialog.shortcutDelegate.model.shortcut = keySequence;
                    }

                    onErrorOccurred: (title, message) => {
                        root.QQC.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                    }

                    onShowStealStandardShortcutDialog: (title, message, sequence) => {
                        stealStandardShortcutDialog.title = title
                        stealStandardShortcutDialog.message = message;
                        stealStandardShortcutDialog.sequence = sequence;
                        stealStandardShortcutDialog.parent = root.QQC.Overlay.overlay;
                        stealStandardShortcutDialog.sequenceItem = this;
                        stealStandardShortcutDialog.open();
                    }
                }

                KeySequenceItem {
                    id: alternateKeySequenceItem
                    Layout.fillWidth: true

                    defaultKeySequence: shortcutDialog.actionDescription?.defaultAlternateShortcut ?? ""

                    label: i18ndc("kirigami-actioncollection", "@label", "Alternative:")

                    onKeySequenceModified: {
                        shortcutDialog.shortcutDelegate.model.alternateShortcut = keySequence;
                    }

                    onErrorOccurred: (title, message) => {
                        root.QQC.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                    }

                    onShowStealStandardShortcutDialog: (title, message, sequence) => {
                        stealStandardShortcutDialog.title = title
                        stealStandardShortcutDialog.message = message;
                        stealStandardShortcutDialog.sequence = sequence;
                        stealStandardShortcutDialog.parent = root.QQC.Overlay.overlay;
                        stealStandardShortcutDialog.sequenceItem = this;
                        stealStandardShortcutDialog.open();
                    }
                }

                QQC.Dialog {
                    id: stealStandardShortcutDialog

                    property string message
                    property var sequence
                    property KeySequenceItem sequenceItem

                    modal: true

                    contentItem: QQC.Label {
                        text: stealStandardShortcutDialog.message
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    standardButtons: Kirigami.PromptDialog.Apply | Kirigami.PromptDialog.Cancel

                    onApplied: {
                        sequenceItem.stealStandardShortcut(sequence);
                        close();
                    }

                    onRejected: close()
                }
            }

            footer: RowLayout {
                QQC.DialogButtonBox {
                    Layout.fillWidth: true
                    standardButtons: QQC.DialogButtonBox.Close | QQC.DialogButtonBox.Reset
                    onRejected: shortcutDialog.close();
                    onReset: {
                        root.model.reset(listView.model.mapToSource(listView.model.index(shortcutDialog.index, 0)));
                        shortcutDialog.keySequence = shortcutDialog.shortcutDelegate.model.shortcut;
                        shortcutDialog.alternateKeySequence = shortcutDialog.shortcutDelegate.model.alternateShortcut;
                    }
                    leftPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    topPadding: Kirigami.Units.smallSpacing
                    rightPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                }
            }
        }

        Kirigami.PlaceholderMessage {
            width: parent.width - Kirigami.Units.gridUnit * 4
            anchors.centerIn: parent
            text: i18ndc("kirigami-actioncollection", "Placeholder message", "No shortcuts found")
            visible: listView.count === 0
        }
    }

    footer: QQC.ToolBar {
        padding: 0

        contentItem: QQC.DialogButtonBox {
            padding: Kirigami.Units.largeSpacing
            standardButtons: QQC.Dialog.Save | QQC.Dialog.Reset

            onAccepted: {
                root.model.save()
                root.closeDialog();
            }
            onReset: root.model.resetAll()
        }
    }
}
