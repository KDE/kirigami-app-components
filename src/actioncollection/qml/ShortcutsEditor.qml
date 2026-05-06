// SPDX-FileCopyrightText: 2024 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC
import QtQuick.Layouts

import org.kde.kitemmodels
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC
import org.kde.ki18n

Kirigami.ScrollablePage {
    id: root

    property alias model: actionModel

    title: _tr.i18ndc("kirigami-actioncollection", "@title:window", "Shortcuts")

    actions: Kirigami.Action {
        displayComponent: Kirigami.SearchField {
            placeholderText: _tr.i18ndc("kirigami-actioncollection", "@label:textbox", "Filter…")
            onTextChanged: searchFilterProxyModel.setFilterFixedString(text);
        }
    }

    KI18nContext {
        id: _tr
        translationDomain: "kirigami-actioncollection"
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
                shownActions: AC.ActionModel.ActiveActions
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

        component ShortcutDelegate : QQC.ItemDelegate {
            id: shortcutDelegate

            required property int index
            // Bind model in order to have editing work
            required property var model

            width: ListView.view.width
            text: model.actionDescription.text.replace(/&([^&])/g, "$1").replace("&&", "&")

            Accessible.description: model.actionDescription.shortcut

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: implicitWidth
                    source: shortcutDelegate.model.actionDescription.icon.name || shortcutDelegate.model.actionDescription.icon.source
                }

                QQC.Label {
                    text: shortcutDelegate.text
                    Layout.fillWidth: true
                    Accessible.ignored: true
                }

                QQC.Label {
                    id: shortcutLabel
                    text: shortcutDelegate.model.shortcut
                    Accessible.ignored: true
                }
                Kirigami.Separator {
                    visible: alternateShortcutLabel.text.length > 0 && shortcutLabel.text.length > 0
                    Layout.fillHeight: true
                }
                QQC.Label {
                    id: alternateShortcutLabel
                    visible: text.length > 0
                    text: shortcutDelegate.model.alternateShortcut
                    Accessible.ignored: true
                }
            }

            onClicked: {
                shortcutDialog.title = _tr.i18ndc("kirigami-actioncollection", "@title:window", "Shortcut: %1",  model.actionDescription.text);
                shortcutDialog.actionDescription = model.actionDescription;
                shortcutDialog.keySequence = model.shortcut;
                shortcutDialog.alternateKeySequence = model.alternateShortcut;
                shortcutDialog.index = shortcutDelegate.index;
                shortcutDialog.shortcutDelegate = shortcutDelegate
                shortcutDialog.open()
            }
        }

        delegate: ShortcutDelegate { }

        QQC.Dialog {
            id: shortcutDialog

            width: Math.max(implicitWidth, Kirigami.Units.gridUnit * 20)
            property AC.ActionData actionDescription
            property alias keySequence: keySequenceItem.keySequence
            property alias alternateKeySequence: alternateKeySequenceItem.keySequence
            // Needs to access shortcutDelegate.model for doing setData on the model
            property ShortcutDelegate shortcutDelegate
            property int index: -1
            modal: true

            parent: root.QQC.Overlay.overlay

            contentItem: ColumnLayout {
                spacing: 0
                KeySequenceItem {
                    id: keySequenceItem
                    Layout.fillWidth: true

                    defaultKeySequence: shortcutDialog.actionDescription?.defaultShortcut ?? ""

                    label: _tr.i18ndc("kirigami-actioncollection", "@label", "Shortcut:")
                    onKeySequenceModified: {
                        print("SETTING", keySequence)
                        shortcutDialog.shortcutDelegate.model.shortcut = keySequence;
                    }

                    onErrorOccurred: (title, message) => {
                        root.QQC.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                    }

                    onShowStealShortcutDialog: (title, message, sequence) => {
                        stealShortcutDialog.title = title
                        stealShortcutDialog.message = message;
                        stealShortcutDialog.sequence = sequence;
                        stealShortcutDialog.parent = root.QQC.Overlay.overlay;
                        stealShortcutDialog.sequenceItem = this;
                        stealShortcutDialog.open();
                    }
                }

                KeySequenceItem {
                    id: alternateKeySequenceItem
                    Layout.fillWidth: true

                    defaultKeySequence: shortcutDialog.actionDescription?.defaultAlternateShortcut ?? ""

                    label: _tr.i18ndc("kirigami-actioncollection", "@label", "Alternative:")

                    onKeySequenceModified: {
                        shortcutDialog.shortcutDelegate.model.alternateShortcut = keySequence;
                    }

                    onErrorOccurred: (title, message) => {
                        root.QQC.ApplicationWindow.showPassiveNotification(title + '\n' + message);
                    }

                    onShowStealShortcutDialog: (title, message, sequence) => {
                        stealShortcutDialog.title = title
                        stealShortcutDialog.message = message;
                        stealShortcutDialog.sequence = sequence;
                        stealShortcutDialog.parent = root.QQC.Overlay.overlay;
                        stealShortcutDialog.sequenceItem = this;
                        stealShortcutDialog.open();
                    }
                }

                QQC.Dialog {
                    id: stealShortcutDialog

                    property string message
                    property var sequence
                    property KeySequenceItem sequenceItem

                    modal: true

                    contentItem: QQC.Label {
                        text: stealShortcutDialog.message
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
            text: _tr.i18ndc("kirigami-actioncollection", "Placeholder message", "No shortcuts found")
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
