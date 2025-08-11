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

private:
    QString m_collection;
};

QML_DECLARE_TYPEINFO(ActionCollectionData, QML_HAS_ATTACHED_PROPERTIES)


struct ActionData {
    QString icon;
    QString shortcut;
};

class ActionCollection : public QObject
{
    Q_OBJECT
    //QML_NAMED_ELEMENT(ActionCollection)
    //QML_UNCREATABLE("Cannot create objects of type ActionCollection")

public:
    explicit ActionCollection(const QString &name, QObject *parent = nullptr);
    ~ActionCollection() override;

    QString name() const;

    void addAction(const QString &name, QObject *action);
    QObject *action(const QString &name);

    QList<QObject *> actions() const;

    QString defaultShortcut(const QString &name) const;

Q_SIGNALS:
    void aboutToAddAction(int position, QObject *action);
    void actionAdded(int position, QObject *action);
    void aboutToRemoveAction(int position, QObject *action);
    void actionRemoved(int position, QObject *action);
    void actionRemoved(QObject *action);

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
        IconNameRole = Qt::UserRole + 1,
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

    Q_INVOKABLE QObject *action(const QString &name);

    ActionCollection *collection() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void nameChanged(const QString &name);

private:
    QPointer<ActionCollection> m_collection;
};

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

