// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#ifndef QMLACTIONCOLLECTION_H
#define QMLACTIONCOLLECTION_H

#include <QObject>
#include <QQmlEngine>
#include <fontconfig/fontconfig.h>
#include <qqmlregistration.h>

#include "actioncollection.h"
#include "actiondata.h"

class ActionCollectionAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString collection READ collection WRITE setCollection NOTIFY collectionChanged FINAL)
    Q_PROPERTY(QString action READ action WRITE setAction NOTIFY actionChanged FINAL)

public:
    explicit ActionCollectionAttached(QObject *parent = nullptr);
    ~ActionCollectionAttached() override;

    QString collection() const;
    void setCollection(const QString &collection);

    QString action() const;
    void setAction(const QString &actionName);

Q_SIGNALS:
    void collectionChanged();
    void actionChanged();
    void shortcutChanged();

private:
    void rebindActionData();

    QString m_collection;
    QString m_actionName;
    QPointer<ActionData> m_action;
};

// Accessible from both C++ and QML
class QmlActionCollection : public ActionCollection
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionCollection)
    QML_ATTACHED(ActionCollectionAttached)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    Q_PROPERTY(QQmlListProperty<ActionData> actions READ actionsProperty CONSTANT FINAL)

    Q_CLASSINFO("DefaultProperty", "actions")

public:
    explicit QmlActionCollection(QObject *parent = nullptr);
    ~QmlActionCollection() override;

    void setName(const QString &name);

    void insertAction(ActionData *action);
    // All actions in this collections that have an active and working Kirigami.Action instance in this moment
    QList<ActionData *> activeActions() const;

    QQmlListProperty<ActionData> actionsProperty();

    // QML attached property
    static ActionCollectionAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void aboutToAddActionInstance(int position, ActionData *action);
    void actionInstanceAdded(int position, ActionData *action);
    void aboutToRemoveActionInstance(int position, ActionData *action);
    void actionInstanceRemoved(int position, ActionData *action);

private:
    static void actions_append(QQmlListProperty<ActionData> *prop, ActionData *object);
    static qsizetype actions_count(QQmlListProperty<ActionData> *prop);
    static ActionData *actions_at(QQmlListProperty<ActionData> *prop, qsizetype index);
    static void actions_clear(QQmlListProperty<ActionData> *prop);
};

QML_DECLARE_TYPEINFO(ActionCollectionAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif
