// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier:  LGPL-2.1-or-later OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Window
import QtQuick.Controls as QQC
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigami.actioncollection as AC
import org.kde.kitemmodels

Kirigami.SearchDialog {
    id: root

    model: KSortFilterProxyModel {
        sourceModel: AC.ActionModel {
            id: actionModel
            shownActions: ActionModel.ActiveActions
        }

        filterRowCallback: function (source_row, source_parent) {
            let instances = sourceModel.data(sourceModel.index(source_row, 0, source_parent), AC.ActionModel.ActionInstancesRole);
            let text = sourceModel.data(sourceModel.index(source_row, 0, source_parent))
            return instances !== null && instances.length > 0 && text.toUpperCase().indexOf(root.text.toUpperCase()) !== -1;
        }

        sortRole: Qt.DisplayRole
        sortCaseSensitivity: Qt.CaseInsensitive
        filterRole: Qt.DisplayRole
        filterString: root.text
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
        id: commandDelegate

        width: ListView.view.width
        required property int index
        required property QtObject actionDescription
        required property var actionInstances
        required property string shortcut
        required property string alternateShortcut

        icon.name: actionDescription.icon.name
        text: actionDescription.text

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Kirigami.Icon {
                implicitWidth: Kirigami.Units.iconSizes.small
                implicitHeight: implicitWidth
                source: actionDescription.icon.name || actionDescription.icon.source
            }

            QQC.Label {
                Layout.fillWidth: true
                text: {
                    let stripped = actionDescription.text.replace(/&([^&])/g, "$1").replace("&&", "&");
                    return stripped;
                }
            }

            QQC.Label {
                id: shortcutLabel
                text: shortcut
                color: Kirigami.Theme.disabledTextColor
            }
            Kirigami.Separator {
                visible: alternateShortcutLabel.text.length > 0 && shortcutLabel.text.length > 0
                Layout.fillHeight: true
            }
            QQC.Label {
                id: alternateShortcutLabel
                visible: text.length > 0
                text: alternateShortcut
                color: Kirigami.Theme.disabledTextColor
            }
        }

        onClicked: {
            actionInstances[0].trigger()
            root.close()
        }
    }

    emptyText: i18ndc("kirigami-addons6", "@info:placeholder", "No results found")
}
