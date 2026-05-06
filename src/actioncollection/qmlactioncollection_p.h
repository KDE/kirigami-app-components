// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#ifndef QMLACTIONCOLLECTION_H
#define QMLACTIONCOLLECTION_H

#include <QObject>
#include <QQmlEngine>
#include <qqmlregistration.h>

#include "actioncollection.h"
#include "actiondata_p.h"

namespace KirigamiActions
{

class ActionCollectionAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString collection READ collection WRITE setCollection NOTIFY collectionChanged FINAL)
    Q_PROPERTY(QVariant action READ action WRITE setAction NOTIFY actionChanged FINAL)

public:
    explicit ActionCollectionAttached(QObject *parent = nullptr);
    ~ActionCollectionAttached() override;

    QString collection() const;
    void setCollection(const QString &collection);

    QVariant action() const;
    void setAction(const QVariant &actionName);

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

/*!
 * \qmltype ActionCollection
 * \inqmlmodule org.kde.kirigami.actioncollection
 * \nativetype ActionCollection
 *
 * \brief ActionCollection manages a set of ActionData objects.
 *
 * It allows them to be grouped for organized presentation of configuration to the user and
 * saving + loading of shortcuts configuration.
 *
 * \code
 * import org.kde.kirigami.actioncollection as AC
 * ...
 * AC.ActionCollection {
 *     name: "EditActions"
 *     AC.ActionData {
 *        name: "exampleAction"
 *        icon.name: "edit-copy"
 *        text: "Copy"
 *        defaultShortcut: StandardKey.Copy
 *     }
 *     ...
 * }
 * ...
 * Kirigami.Action {
 *   id: copyAction
 *   AC.ActionCollection.collection: "EditActions"
 *   AC.ActionCollection.action: "exampleAction"
 *   onTriggered: {
 *       ...
 *   }
 * }
 * \endcode
 */
class QmlActionCollection : public ActionCollection
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionCollection)
    QML_ATTACHED(ActionCollectionAttached)

    /*!
     * \qmlproperty string name
     *
     * The unique name of the collection within the whole application.
     * It is required and should be set only once
     */
    // FIXME: this should be REQUIRED but not in standardactioncollection
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /*!
     * \qmlproperty list<ActionData> actions
     *
     * All the actions this collection contains.
     * It's the default property, so actionData instances can be declared just as children.
     * \code
     * AC.ActionCollection {
     *     name: "EditActions"
     *     AC.ActionData {
     *        name: "exampleAction"
     *        icon.name: "edit-copy"
     *        text: "Copy"
     *        defaultShortcut: StandardKey.Copy
     *     }
     *     AC.ActionData {
     *        name: "backAction"
     *        icon.name: "go previous"
     *        text: "Back"
     *     }
     *     ...
     * }
     * \endcode
     */
    Q_PROPERTY(QQmlListProperty<KirigamiActions::ActionData> actions READ actionsProperty CONSTANT FINAL)

    Q_CLASSINFO("DefaultProperty", "actions")

public:
    explicit QmlActionCollection(QObject *parent = nullptr);
    ~QmlActionCollection() override;

    void setName(const QString &name);

    void insertAction(ActionData *action);
    // All actions in this collections that have an active and working Kirigami.Action instance in this moment
    QList<ActionData *> activeActions() const;

    QQmlListProperty<KirigamiActions::ActionData> actionsProperty();

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

}

QML_DECLARE_TYPEINFO(KirigamiActions::ActionCollectionAttached, QML_HAS_ATTACHED_PROPERTIES)

#endif
