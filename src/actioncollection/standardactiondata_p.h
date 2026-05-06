// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#pragma once

#include "actiondata_p.h"

namespace KirigamiActions
{

/*!
 * \qmltype StandardActionData
 * \inqmlmodule org.kde.kirigami.actioncollection
 * \nativetype QAction
 *
 * \brief Declarative representation for a named action derivarive from
 * KStandardActions
 *
 * This element needs to always be declared as a child of ActionCollection
 *
 * \code
 * import org.kde.kirigami.actioncollection as AC
 * ...
 * AC.ActionCollection {
 *     name: "EditActions"
 *     AC.StandardActionData {
 *        standardAction: AC.StandardActionData.Copy
 *     }
 *     ...
 * }
 * ...
 * Kirigami.Action {
 *   id: copyAction
 *   AC.ActionCollection.collection: "EditActions"
 *   AC.ActionCollection.action: AC.StandardActionData.Copy
 *   onTriggered: {
 *       ...
 *   }
 * }
 * \endcode
 */
class StandardActionData : public ActionData
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(StandardAction standardAction READ standardAction WRITE setStandardAction NOTIFY standardActionChanged FINAL REQUIRED)

public:
    enum StandardAction {
        ActionNone,
        // File Menu
        New,
        Open,
        OpenRecent,
        Save,
        SaveAs,
        Revert,
        Close,
        Print,
        PrintPreview,
        Mail,
        Quit,
        // Edit Menu
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        SelectAll,
        Deselect,
        Find,
        FindNext,
        FindPrev,
        Replace,
        // View Menu
        ActualSize,
        FitToPage,
        FitToWidth,
        FitToHeight,
        ZoomIn,
        ZoomOut,
        Zoom,
        Redisplay,
        // Go Menu
        Up,
        Back,
        Forward,
        Home,
        Prior,
        Next,
        Goto,
        GotoPage,
        GotoLine,
        FirstPage,
        LastPage,
        DocumentBack,
        DocumentForward,
        // Bookmarks Menu
        AddBookmark,
        EditBookmarks,
        // Tools Menu
        Spelling,
        // Settings Menu
        ShowMenubar,
        ShowToolbar,
        ShowStatusbar,
        KeyBindings,
        Preferences,
        ConfigureToolbars,
        // Help Menu
        HelpContents,
        WhatsThis,
        ReportBug,
        AboutApp,
        AboutKDE,
        // Other standard actions
        ConfigureNotifications,
        FullScreen,
        Clear,
        SwitchApplicationLanguage,
        DeleteFile,
        RenameFile,
        MoveToTrash,
        Donate,
        HamburgerMenu
        // To keep in sync with KConfigWidgets::KStandardAction
    };
    Q_ENUM(StandardAction)

    explicit StandardActionData(QObject *parent = nullptr);
    ~StandardActionData() override;

    StandardAction standardAction() const;
    void setStandardAction(StandardAction standardAction);

Q_SIGNALS:
    void standardActionChanged(StandardAction standardAction);

private:
    StandardAction m_standardAction = ActionNone;
};

}
