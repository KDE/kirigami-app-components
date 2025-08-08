// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"

#include <QSettings>
#include <QKeySequence>

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>

Q_GLOBAL_STATIC(ActionCollectionStorage, s_actionCollectionStorage)

ActionCollectionData::ActionCollectionData(QObject *parent)
    : QObject(parent)
{
}

ActionCollectionData::~ActionCollectionData()
{}

QString ActionCollectionData::collection() const
{
    return m_collection;
}

void ActionCollectionData::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;

    if (!parent()->objectName().isEmpty()) {
        ActionCollectionStorage::self()->collection(collection)->addAction(parent()->objectName(), parent());
    }

    Q_EMIT collectionChanged();
}

ActionCollectionData *ActionCollectionData::qmlAttachedProperties(QObject *object)
{
    return new ActionCollectionData(object);
}

//////////////////////////////////

ActionCollection::ActionCollection(const QString &name, QObject *parent)
    : QObject(parent)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral(":/actionsrc"));
    KConfigGroup rootCg(config, name);

    for (const QString &grp : rootCg.groupList()) {
        KConfigGroup cg(&rootCg, grp);
        m_shortcuts[grp] = QKeySequence(cg.readEntry("shortcut", QString()));
    }

    qWarning()<<m_shortcuts;

}

ActionCollection::~ActionCollection()
{}

void ActionCollection::addAction(const QString &name, QObject *action)
{
    qWarning() << "adding action" << name << action;
    m_actions[name] = action;
}

QObject *ActionCollection::action(const QString &name)
{
    return m_actions.value(name);
}

/////////////////////////////////

ActionCollectionStorage::ActionCollectionStorage(QObject *parent)
    : QObject(parent)
{}

ActionCollectionStorage::~ActionCollectionStorage()
{}

ActionCollectionStorage *ActionCollectionStorage::self()
{
    return s_actionCollectionStorage;
}

ActionCollection *ActionCollectionStorage::collection(const QString &name)
{
    ActionCollection *coll = nullptr;
    if (!m_collections.contains(name)) {
        coll = new ActionCollection(name, this);
        m_collections[name] = coll;
        return coll;
    }

    return m_collections[name];
}


#include "moc_actioncollection.cpp"
