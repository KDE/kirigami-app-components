// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#ifndef ACTIONCOLLECTION_P_H
#define ACTIONCOLLECTION_P_H

#include <KStandardActions>
#include <QHash>
#include <QString>

using namespace Qt::StringLiterals;

namespace KirigamiActions
{

class ActionData;

class ActionCollectionPrivate
{
public:
    ActionCollectionPrivate();
    ~ActionCollectionPrivate();
    QString m_name;
    QString m_text;
    QHash<QString, ActionData *> m_actionMap;
    QList<ActionData *> m_actions;
    QList<ActionData *> m_activeActions;
};

// Copied from kstandardactions_p.h
struct StandardActionsInfo {
    KStandardActions::StandardAction id;
    KStandardShortcut::StandardShortcut shortcut;
    const QString name;
    const char *text;
    const char *tooltip;
    const QString iconName;
};

}

#endif
