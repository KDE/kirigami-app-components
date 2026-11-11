// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.sonnet as Sonnet

Kirigami.ScrollablePage {
    id: root

    readonly property Sonnet.Settings settings: Sonnet.Settings {
        id: settings
    }

    readonly property Kirigami.PageRow pageStack: QQC2.ApplicationWindow.window.pageStack

    KI18nContext {
        id: _tr
        translationDomain: "kirigami-app-components"
    }

    function add(word: string): void {
        const dictionary = root.settings.currentIgnoreList;
        dictionary.push(word);
        root.settings.currentIgnoreList = dictionary;
    }

    function remove(word: string): void {
        root.settings.currentIgnoreList = root.settings.currentIgnoreList.filter(value => value !== word);
    }

    Kirigami.Form {
        Kirigami.FormGroup {
            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    text: _tr.i18nc("@label:checkbox", "Enable automatic spell checking")
                    checked: root.settings.checkerEnabledByDefault
                    onToggled: {
                        root.settings.checkerEnabledByDefault = checked;
                        root.settings.save();
                    }
                }
            }

            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    text: _tr.i18nc("@label:checkbox", "Ignore uppercase words")
                    checked: root.settings.skipUppercase
                    onToggled: {
                        root.settings.skipUppercase = checked;
                        root.settings.save();
                    }
                }
            }

            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    text: _tr.i18nc("@label:checkbox", "Ignore hyphenated words")
                    checked: root.settings.skipRunTogether
                    onToggled: {
                        root.settings.skipRunTogether = checked;
                        root.settings.save();
                    }
                }
            }

            Kirigami.FormEntry {
                contentItem: QQC2.CheckBox {
                    id: autodetectLanguageCheckbox
                    text: _tr.i18nc("@label:checkbox", "Detect language automatically")
                    checked: root.settings.autodetectLanguage
                    onToggled: {
                        root.settings.autodetectLanguage = checked;
                        root.settings.save();
                    }
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormEntry {
                title: _tr.i18nc("@label:listbox", "Selected default language:")

                contentItem: QQC2.ComboBox {
                    id: languageComboBox

                    property bool isEmpty: false

                    model: languageComboBox.isEmpty ? [{"display": _tr.i18nc("No selected language", "None")}] : root.settings.dictionaryModel
                    textRole: "display"
                    valueRole: "languageCode"

                    Component.onCompleted: {
                        if (root.settings.dictionaryModel.rowCount() === 0) {
                            languageComboBox.isEmpty = true;
                        } else {
                            languageComboBox.currentIndex = languageComboBox.indexOfValue(root.settings.defaultLanguage);
                        }
                    }
                    onActivated: {
                        root.settings.defaultLanguage = languageComboBox.currentValue;
                        root.settings.save();
                    }
                }
            }

            Kirigami.FormAction {
                action: Kirigami.Action {
                    text: _tr.i18nc("@label:listbox", "Additional Spell Checking Languages")
                    onTriggered: root.pageStack.pushDialogLayer(spellCheckingLanguageList, {}, {
                        width: root.pageStack.width - Kirigami.Units.gridUnit * 5,
                        height: root.pageStack.height - Kirigami.Units.gridUnit * 5,
                    })
                }
                subtitle: _tr.i18n("%1 will provide spell checking and suggestions for the languages listed here when autodetection is enabled.", Qt.application.displayName)
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                action: Kirigami.Action {
                    text: _tr.i18nc("@action:button", "Open Personal Dictionary")
                    onTriggered: root.pageStack.pushDialogLayer(dictionaryPage, {}, {
                        width: root.pageStack.width - Kirigami.Units.gridUnit * 5,
                        height: root.pageStack.height - Kirigami.Units.gridUnit * 5,
                    })
                }
            }
        }
    }

    Component {
        id: spellCheckingLanguageList

        Kirigami.ScrollablePage {
            id: scroll
            title: _tr.i18nc("@title:window", "Spell checking languages")
            enabled: autodetectLanguageCheckbox.checked
            ListView {
                clip: true
                model: root.settings.dictionaryModel
                delegate: Kirigami.CheckSubtitleDelegate {
                    required property var model
                    width: ListView.view.width

                    text: model.display
                    action: Kirigami.Action {
                        onTriggered: model.checked = checked
                    }
                    Accessible.description: model.isDefault ? _tr.i18n("Default Language") : ''
                    checked: model.checked

                    icon.source: model.isDefault ? "favorite" : undefined
                }
            }
        }
    }

    Component {
        id: dictionaryPage

        Kirigami.ScrollablePage {
            title: _tr.i18nc("@title:window", "Spell checking dictionary")
            footer: QQC2.ToolBar {
                contentItem: RowLayout {
                    QQC2.TextField {
                        id: dictionaryField
                        Layout.fillWidth: true
                        Accessible.name: placeholderText
                        placeholderText: _tr.i18nc("Placeholder message", "Add a new word to your personal dictionary…")
                    }
                    QQC2.Button {
                        text: _tr.i18nc("@action:button", "Add Word")
                        icon.name: "list-add"
                        enabled: dictionaryField.text.length > 0
                        onClicked: {
                            root.add(dictionaryField.text);
                            dictionaryField.clear();
                            root.settings.save();
                        }
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                    }
                }
            }
            ListView {
                topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
                bottomMargin: Math.round(Kirigami.Units.smallSpacing / 2)

                model: root.settings.currentIgnoreList
                delegate: QQC2.ItemDelegate {
                    id: wordDelegate
                    width: ListView.view.width

                    required property string modelData

                    text: modelData

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        QQC2.Label {
                            text: wordDelegate.text
                            elide: Text.ElideRight
                            Layout.fillWidth: true
                        }

                        QQC2.ToolButton {
                            text: _tr.i18nc("@action:button", "Delete word")
                            icon.name: "delete"
                            display: QQC2.ToolButton.IconOnly
                            onClicked: {
                                root.remove(wordDelegate.modelData);
                                root.settings.save();
                            }

                            QQC2.ToolTip.text: text
                            QQC2.ToolTip.visible: hovered
                            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
                        }
                    }
                }
            }
        }
    }
}
