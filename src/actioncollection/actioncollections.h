// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#ifndef ACTIONCOLLECTIONS_H
#define ACTIONCOLLECTIONS_H

#include <QObject>
#include <QQmlEngine>
#include <qqmlregistration.h>

#include "kirigamiactioncollection_export.h"

namespace KirigamiActions
{

class ActionCollection;
class ActionCollectionsPrivate;

/*!
 * \class Kirigami::ActionCollection::ActionCollections
 * \inheaderfile Kirigami/ActionCollection/ActionCollections
 * \inmodule ActionCollection
 *
 * \brief This Singleton class contains all the ActionCollection
 * instances used in this application, retrievable by their unique name.
 */
class KIRIGAMIACTIONCOLLECTION_EXPORT ActionCollections : public QObject
{
    Q_OBJECT

public:
    /*!
     * This shouldn't be explicitly called, rather the single global
     * instance accessed with ActionCollections::self()
     */
    explicit ActionCollections(QObject *parent = nullptr);
    ~ActionCollections() override;

    /*!
     * Returns the global singleton instance of this class.
     * Always use this instead of manually creating one.
     */
    static ActionCollections *self();

    /*!
     * Creates a new collection identified by the given unique name,
     * and the given user readable text label.
     * Returns the created collection, or nullptr if a collection with the
     * given name was already present.
     */
    ActionCollection *createCollection(const QString &name, const QString &text);

    /*!
     * Returns the ActionCollection instance uniquely identified by the provided name.
     * If no collection with this name exists, nullptr will be returned.
     */
    ActionCollection *collection(const QString &name);

    /*!
     * Returns the list of evenry ActionCollection existing in the application.
     */
    QList<ActionCollection *> collections();

Q_SIGNALS:
    /*!
     * Emitted when a new ActionCollection instance has been created.
     */
    void collectionInserted(KirigamiActions::ActionCollection *collection);

    /*!
     * Emitted when an ActionCollection instance is being destroyed or
     * made otherwise unavailable.
     */
    void collectionRemoved(KirigamiActions::ActionCollection *collection);

private:
    std::unique_ptr<ActionCollectionsPrivate> d;
    friend class ActionModelPrivate;
    friend class QmlActionCollection;
};

}

#endif
