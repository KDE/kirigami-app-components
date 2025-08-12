// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <qqmlregistration.h>
#include <QQmlEngine>

class ActionCollectionData : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionCollection)
    QML_ATTACHED(ActionCollectionData)
    QML_UNCREATABLE("Cannot create objects of type ActionCollectionData, use it as an attached property")

    Q_PROPERTY(QString collection READ collection WRITE setCollection NOTIFY collectionChanged FINAL)

public:
    explicit ActionCollectionData(QObject *parent = nullptr);
    ~ActionCollectionData() override;

    QString collection() const;
    void setCollection(const QString &collection);

    // QML attached property
    static ActionCollectionData *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void collectionChanged();
    void shortcutChanged();

private:
    QString m_collection;
};

QML_DECLARE_TYPEINFO(ActionCollectionData, QML_HAS_ATTACHED_PROPERTIES)


struct ActionData {
    QString name;
    QString text;
    QString icon;
    QString defaultShortcut;
    QString shortcut;
};


// C++ only api
class ActionCollection : public QObject
{
    Q_OBJECT

public:
    explicit ActionCollection(const QString &name, QObject *parent = nullptr);
    ~ActionCollection() override;

    QString name() const;

    void addActionInstance(const QString &name, QObject *action);
    QObject *actionInstance(const QString &name);

    QList<QObject *> actions() const;
    QList <ActionData> actionDescriptions() const;

    // TODO: perhaps on the c++ facing api expose only QKeySequence
    void setShortcut(const QString &name, const QString &shortcut);
    QString shortcut(const QString &name) const;

    QString defaultShortcut(const QString &name) const;

Q_SIGNALS:
    void aboutToAddAction(int position, QObject *action);
    void actionAdded(int position, QObject *action);
    void aboutToRemoveAction(int position, QObject *action);
    void actionRemoved(int position, QObject *action);
    void actionRemoved(QObject *action);
    void shortcutChanged(const QString &shortcut);

private:
    QString m_name;
    QMap<QString, QObject*> m_actions;
    QHash<QString, ActionData> m_actionData;
};

class ActionCollectionModel : public QAbstractListModel {
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionCollectionModel)
    // TODO: this should be able to have multiple names and multiple collections
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

public:
    enum Role {
        ActionNameRole = Qt::UserRole + 1,
        IconNameRole,
        ShortcutRole,
        ShortcutDisplayRole,
        DefaultShortcutRole,
        AlternateShortcutsRole,
        CollectionNameRole,
    };

    explicit ActionCollectionModel(QObject *parent = nullptr);
    ~ActionCollectionModel() override;

    QString name() const;
    void setName(const QString &name);

    // Somewhere outside ActionCollectionModel ?
    Q_INVOKABLE void setShortcut(const QString &name, const QString &shortcut);

    ActionCollection *collection() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void nameChanged(const QString &name);

private:
    QPointer<ActionCollection> m_collection;
};

// C++ only api
class ActionCollectionStorage : public QObject
{
    Q_OBJECT

public:
    explicit ActionCollectionStorage(QObject *parent = nullptr);
    ~ActionCollectionStorage() override;

    static ActionCollectionStorage *self();

    ActionCollection *collection(const QString &name);

private:
    QHash<QString, ActionCollection*> m_collections;
};

