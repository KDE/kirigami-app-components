// SPDX-FileCopyrightText: 2022 Joshua Goins <josh@redstrate.com>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Window
import QtQuick.Layouts
import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.coreaddons as Core

Kirigami.ScrollablePage {
    id: page

    title: KI18n.i18nc("@title:window", "About KDE")

    readonly property string _bugReportUrl: {
        const bugAddress = Core.AboutData.bugAddress ?? "";
        if (bugAddress !== "submit@bugs.kde.org") {
            // A plain email address can only be opened with a mailto: scheme.
            if (bugAddress.includes("@") && !bugAddress.includes(":")) {
                return "mailto:" + bugAddress;
            }
            return bugAddress;
        }
        const elements = Core.AboutData.productName.split('/');
        let url = `https://bugs.kde.org/enter_bug.cgi?format=guided&product=${elements[0]}&version=${Core.AboutData.version}`;
        if (elements.length === 2) {
            url += "&component=" + elements[1];
        }
        return url;
    }

    Kirigami.Form {
        Layout.topMargin: Kirigami.Units.largeSpacing * 4

        Kirigami.FormGroup {
            Kirigami.FormEntry {
                id: generalDelegate
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing * 2

                    Kirigami.Icon {
                        Layout.preferredHeight: Kirigami.Units.iconSizes.huge
                        Layout.preferredWidth: height
                        Layout.maximumWidth: page.width / 3;
                        Layout.rightMargin: Kirigami.Units.largeSpacing
                        source: "kde"
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing

                        Kirigami.Heading {
                            Layout.fillWidth: true
                            text: KI18n.i18nc("KDE title besides logo in the About KDE page", "KDE")
                            wrapMode: Text.WordWrap
                        }

                        Kirigami.Heading {
                            Layout.fillWidth: true
                            level: 3
                            type: Kirigami.Heading.Type.Secondary
                            wrapMode: Text.WordWrap
                            text: KI18n.i18nc("KDE motto in About KDE page", "Be Free!")
                        }
                    }
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormEntry {
                contentItem: QQC2.Label {
                    Layout.fillWidth: true
                    text: KI18n.i18nc("KDE community description", "KDE is a world-wide community of software engineers, artists, writers, translators and creators who are committed to Free Software development. KDE produces the Plasma desktop environment, hundreds of applications, and the many software libraries that support them.\n\n\
KDE is a cooperative enterprise: no single entity controls its direction or products. Instead, we work together to achieve the common goal of building the world's finest Free Software. Everyone is welcome to join and contribute to KDE, including you.")
                    wrapMode: Text.WordWrap
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    icon.name: "globe-symbolic"
                    text: KI18n.i18nc("Link to kde.org", "Homepage")
                    onTriggered: Qt.openUrlExternally("https://kde.org/")
                }
            }
        }

        Kirigami.FormGroup {
            title: KI18n.i18nc("Form bugs section title", "Report bugs")
            visible: page._bugReportUrl.length > 0

            Kirigami.FormEntry {
                contentItem: QQC2.Label {
                    Layout.fillWidth: true
                    text: KI18n.i18nc("Explanation about reporting bugs", "Software can always be improved, and the KDE team is ready to do so. However, you - the user - must tell us when something does not work as expected or could be done better.\n\n\
KDE has a bug tracking system. Use the button below to file a bug, or use the program's About page to report a bug specific to this application.\n\n\
If you have a suggestion for improvement then you are welcome to use the bug tracking system to register your wish. Make sure you use the severity called \"Wishlist\".")
                    wrapMode: Text.WordWrap
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    icon.name: "tools-report-bug-symbolic"
                    text: KI18n.i18nc("Link to bugzilla", "Report a bug")
                    onTriggered: Qt.openUrlExternally(page._bugReportUrl)
                }
            }
        }

        Kirigami.FormGroup {
            title: KI18n.i18nc("Form section title about getting involved", "Join us")

            Kirigami.FormEntry {
                contentItem: QQC2.Label {
                    Layout.fillWidth: true
                    text: KI18n.i18nc("Descrioption of roles one can get involved as", "You do not have to be a software developer to be a member of the KDE team. You can join the language teams that translate program interfaces. You can provide graphics, themes, sounds, and improved documentation. You decide!")
                    wrapMode: Text.WordWrap
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    icon.name: "system-user-list"
                    text: KI18n.i18nc("Link to wiki page get involved", "Get Involved")
                    onTriggered: Qt.openUrlExternally("https://community.kde.org/Get_Involved")
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    icon.name: 'applications-development-symbolic'
                    text: KI18n.i18nc("Link to develop.kde.org", "Developer Documentation")
                    onTriggered: Qt.openUrlExternally("https://develop.kde.org/")
                }
            }
        }

        Kirigami.FormGroup {
            title: KI18n.i18nc("Form section title about donations", "Support us")

            Kirigami.FormEntry {
                contentItem: QQC2.Label {
                    Layout.fillWidth: true
                    text: KI18n.i18nc("Text about donations to KDE", "KDE software is and will always be available free of charge, however creating it is not free.\n\n\
To support development the KDE community has formed the KDE e.V., a non-profit organization legally founded in Germany. KDE e.V. represents the KDE community in legal and financial matters.\n\n\
KDE benefits from many kinds of contributions, including financial. We use the funds to reimburse members and others for expenses they incur when contributing. Further funds are used for legal support and organizing conferences and meetings.\n\n\
We would like to encourage you to support our efforts with a financial donation.\n\n\
Thank you very much in advance for your support.")
                    wrapMode: Text.WordWrap
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    text: KI18n.i18nc("link to ev.kde.org", "KDE e.V.")
                    icon.name: 'kde-symbolic'
                    onTriggered: Qt.openUrlExternally("https://ev.kde.org/")
                }
            }

            Kirigami.FormSeparator {}

            Kirigami.FormAction {
                triggerIcon.name: "open-link-symbolic"
                action: Kirigami.Action {
                    text: KI18n.i18nc("Link to donation page", "Donate")
                    icon.name: 'donate-symbolic'
                    // Lets KDE know which application the donation came from, like AboutPage does.
                    onTriggered: {
                        if (Core.AboutData.componentName.length > 0) {
                            Qt.openUrlExternally("https://www.kde.org/donate?app=" + encodeURIComponent(Core.AboutData.componentName))
                        } else {
                            Qt.openUrlExternally("https://www.kde.org/donate")
                        }
                    }
                }
            }
        }
    }
}
