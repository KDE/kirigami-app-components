// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"

#include <QSettings>
#include <QKeySequence>
#include <QQmlProperty>

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
        ActionData a;
        a.icon = cg.readEntry("icon", QString());
        a.shortcut = cg.readEntry("shortcut", QString());
        m_actionData[grp] = a;
        qWarning() << grp << a.icon << a.shortcut;
    }
}

ActionCollection::~ActionCollection()
{}

void ActionCollection::addAction(const QString &name, QObject *action)
{
    if (!action) {
        return;
    }
    qWarning() << "adding action" << name << action;
    m_actions[name] = action;

    ActionData data = m_actionData.value(name);
    if (!data.icon.isEmpty()) {
        QQmlProperty property(action, "icon.name");
        property.write(data.icon);
    }
    if (data.shortcut.isEmpty()) {
        return;
    }

    QMetaEnum me = QMetaEnum::fromType<QKeySequence::StandardKey>();

    // Is the default shortcut a named one, such as "Copy", "Back" etc ?
    for (int i = 0; i < me.keyCount(); ++i) {
        if (QString::fromUtf8(me.key(i)).toLower() == data.shortcut.toLower()) {
            QQmlProperty property(action, "shortcut");
            property.write(QKeySequence::StandardKey(me.value(i)));
            return;
        }
    }

    QQmlProperty property(action, "shortcut");
    property.write(data.shortcut);
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
