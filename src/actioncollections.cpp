// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#include "actioncollections.h"
#include "actioncollections_p.h"
#include "actiondata_p.h"

#include <KLocalizedString>
#include <KStandardShortcut>
#include <QCoreApplication>
#include <QKeySequence>
#include <QQmlContext>
#include <QQmlProperty>
#include <QtQml/qqmlinfo.h>

using namespace Qt::StringLiterals;

Q_GLOBAL_STATIC(ActionCollections, s_actionCollectionStorage)

ActionCollectionsPrivate::ActionCollectionsPrivate(ActionCollections *collections)
    : q(collections)
{
}

void ActionCollectionsPrivate::insertCollection(QmlActionCollection *collection)
{
    if (m_collections.contains(collection->name())) {
        QQmlError error;
        error.setDescription(QStringLiteral("Error: collection name %1 already present.").arg(collection->name()));
        error.setMessageType(QtFatalMsg);
        QQmlContext *context = qmlContext(q);
        if (context) {
            error.setUrl(context->baseUrl());
        }
        qmlWarning(nullptr, error);
        qFatal();
    }

    // This can happen when renaming a collection
    const QStringList keys = m_collections.keys(collection);
    Q_ASSERT(keys.length() < 2);
    if (keys.length() > 0) {
        m_collections.remove(keys.first());
    }

    m_collections.insert(collection->name(), collection);

    QObject::connect(collection, &QObject::destroyed, q, [this, collection]() {
        for (auto it = m_collections.begin(); it != m_collections.end(); ++it) {
            if (it.value() == collection) {
                m_collections.erase(it);
                break;
            }
        }
        Q_EMIT q->collectionRemoved(collection);
    });

    Q_EMIT q->collectionInserted(collection);
}

ActionCollections::ActionCollections(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<ActionCollectionsPrivate>(this))
{
}

ActionCollections::~ActionCollections()
{
}

ActionCollections *ActionCollections::self()
{
    return s_actionCollectionStorage;
}

ActionCollection *ActionCollections::collection(const QString &name)
{
    return d->m_collections.value(name);
}

ActionCollection *ActionCollections::createCollection(const QString &name, const QString &text)
{
    if (d->m_collections.contains(name)) {
        return nullptr;
    }

    QmlActionCollection *ac = new QmlActionCollection(this);
    ac->setName(name);
    ac->setText(text);
    d->m_collections[name] = ac;
    return ac;
}

QList<ActionCollection *> ActionCollections::collections()
{
    QList<ActionCollection *> colls;
    std::copy(d->m_collections.constBegin(), d->m_collections.constEnd(), std::back_inserter(colls));
    return colls;
}

#include "moc_actioncollections.cpp"
