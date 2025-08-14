// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "shortcutsmodel.h"

#include <QSettings>
#include <QKeySequence>
#include <QQmlProperty>

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

// TODO: get rid of this
QString decodeStandardShortcut2(const QString &shortcut)
{
    // Is the default shortcut a named one, such as "Copy", "Back" etc ?
    QMetaEnum me = QMetaEnum::fromType<QKeySequence::StandardKey>();
    for (int i = 0; i < me.keyCount(); ++i) {
        if (QString::fromUtf8(me.key(i)).toLower() == shortcut) {
            return QKeySequence(QKeySequence::StandardKey(me.value(i))).toString();
        }
    }

    return shortcut;
}

ShortcutsModel::ShortcutsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ShortcutsModel::~ShortcutsModel()
{}

QString ShortcutsModel::name() const
{
    if (m_collection) {
        return m_collection->name();
    }

    return {};
}

void ShortcutsModel::setName(const QString &name)
{
    if (m_collection && name == m_collection->name()) {
        return;
    }

    m_collection = ActionCollectionStorage::self()->collection(name);

    connect(m_collection, &ActionCollection::aboutToAddAction,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                beginInsertRows(QModelIndex(), position, position);
            });
    connect(m_collection, &ActionCollection::actionAdded,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                Q_UNUSED(action);
                endInsertRows();
            });
    connect(m_collection, &ActionCollection::aboutToRemoveAction,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                beginRemoveRows(QModelIndex(), position, position);
            });
    connect(m_collection, &ActionCollection::actionAdded,
            this, [this](int position, QObject *action) {
                Q_UNUSED(position);
                Q_UNUSED(action);
                endRemoveRows();
            });

    Q_EMIT nameChanged(m_collection ? name : QString());
}

void ShortcutsModel::setShortcut(const QString &name, const QString &shortcut)
{
    if (!m_collection) {
        return;
    }
    m_collection->setShortcut(name, shortcut);
}

ActionCollection *ShortcutsModel::collection() const
{
    return m_collection;
}

int ShortcutsModel::rowCount(const QModelIndex &parent) const
{
    if (!m_collection || parent.isValid()) {
        return 0;
    }

    return m_collection->actionDescriptions().count();
}

QVariant ShortcutsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    if (!m_collection) {
        return {};
    }

    ActionData ad = m_collection->actionDescriptions()[index.row()];

    switch (role) {
    case Qt::DisplayRole:
        return ad.text;
    case ActionNameRole:
        return ad.name;
    case IconNameRole:
        return ad.icon;
    case DefaultShortcutRole:
        return decodeStandardShortcut2(ad.defaultShortcut);
    case ShortcutRole:
        return decodeStandardShortcut2(ad.shortcut);
    }

    return {};
}

QHash<int, QByteArray> ShortcutsModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" },
        { ActionNameRole, "actionName" },
        { IconNameRole, "iconName" },
        { DefaultShortcutRole, "defaultShortcut" },
        { ShortcutRole, "shortcut" },
        { ShortcutDisplayRole, "shortcutDisplay" },
        { AlternateShortcutsRole, "alternateShortcuts" },
        { CollectionNameRole, "collectionName" },
    };
}


#include "moc_shortcutsmodel.cpp"
