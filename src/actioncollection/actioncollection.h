// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <KStandardActions>
#include <QAction>
#include <QObject>
#include <QQmlEngine>
#include <qqmlregistration.h>

#include "kirigamiactioncollection_export.h"

namespace KirigamiActions
{

class ActionCollectionPrivate;

/*!
 * \class KirigamiActions::ActionCollection
 * \inheaderfile Kirigami/ActionCollection/ActionCollection
 * \inmodule ActionCollection
 *
 * \brief This class is the C++ entry point to access QAction
 * instabces created by the ActionCollection QML module.
 * With this is possible to define new actions on  the C++ side
 * or access ones created from QML, to either connect to them or
 * change proeprties.
 */
class KIRIGAMIACTIONCOLLECTION_EXPORT ActionCollection : public QObject
{
    Q_OBJECT
    /*!
     * \qmlproperty string ActionCollection::text
     *
     * User-readable label for this collection.
     * This will be visible in the shortcut configuration page.
     */
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)

public:
    ~ActionCollection() override;

    /*!
     * Returns the unique name for this collection.
     * The name is guaranteed to be unique in the context of the whole application
     */
    QString name() const;

    /*!
     * Returns the user readable text label for this collection.
     * This will be visible in the shortcut configuration page.
     */
    QString text() const;
    /*!
     * Sets the user readable text label for this collection.
     * This will be visible in the shortcut configuration page.
     */
    void setText(const QString &text);

    /*!
     * Creates a new action and inserts it in this collection.
     *
     * \a name The unique name for the new action. No actions with that name should exist.
     * \a iconName Name for the icon: should be a name present in the current icon theme.
     * \a text The user readable text label for the new action.
     *
     * Returns the newly created QAction, or nullptr if an action with the same name was already present.
     */
    QAction *createAction(const QString &name, const QString &iconName, const QString &text);

    /*!
     * Creates a new action and inserts it in this collection.
     *
     * \a standardAction The KStandardActions::StandardAction we want to add: text and icon of the action
     *                   will be automatically set
     *
     * Returns the newly created QAction, or nullptr if an action with the same name was already present.
     */
    QAction *createAction(KStandardActions::StandardAction standardAction);

    /*!
     * Returns an action given its unique name.
     * If no action with that name exist, the function will return nullptr
     *
     * \a name The unique name fir the action.
     */
    Q_INVOKABLE QAction *action(const QString &name);

    /*!
     * Returns an action based on the given standardAction
     * IF no action based of the standardAction is present, the function will
     * return nullptr
     *
     * \a standardAction the standard action we want to search in the collection
     */
    QAction *action(KStandardActions::StandardAction standardAction);

    /*!
     * All known actions in this collection.
     * */
    QList<QAction *> actions() const;

Q_SIGNALS:
    /*!
     * Emitted when the user-reabable text label for this collection has changed.
     */
    void textChanged(const QString &text);

    /*!
     * Emitted when a new action has been inserted in this collection
     * \a action the new action
     */
    void actionInserted(QAction *action);

    /*!
     * Emitted when an action has been removed from this collection
     * \a action the old action
     */
    void actionRemoved(QAction *action);

protected:
    explicit ActionCollection(QObject *parent = nullptr);

private:
    std::unique_ptr<ActionCollectionPrivate> d;
    friend class QmlActionCollection;
};

class ActionCollectionsPrivate;

}

#endif
