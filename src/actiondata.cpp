// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actiondata.h"

class ActionDataPrivate
{
public:
    ActionDataPrivate()
    {
    }

    QString m_name;
};

ActionData::ActionData(QObject *parent)
    : QAction(parent)
    , d(new ActionDataPrivate())
{
}

ActionData::~ActionData()
{
    delete d;
}

QString ActionData::name() const
{
    return d->m_name;
}

void ActionData::setName(const QString &name)
{
    if (d->m_name == name) {
        return;
    }

    d->m_name = name;

    Q_EMIT nameChanged(name);
}

#include "moc_actiondata.cpp"
