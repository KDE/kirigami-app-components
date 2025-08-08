// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

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


class ActionCollection : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionCollection)
    QML_UNCREATABLE("Cannot create objects of type ActionCollection")

public:
    explicit ActionCollection(const QString &name, QObject *parent = nullptr);
    ~ActionCollection() override;

    void addAction(const QString &name, QObject *action);
    QObject *action(const QString &name);

private:
    QHash<QString, QObject*> m_actions;
    QHash<QString, QKeySequence> m_shortcuts;
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

