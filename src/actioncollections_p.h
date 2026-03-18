// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef ACTIONCOLLECTIONSP_H
#define ACTIONCOLLECTIONSP_H

#include "actioncollections.h"
#include "qmlactioncollection_p.h"

#include <QMap>
#include <QString>

class ActionCollectionsPrivate
{
public:
    ActionCollectionsPrivate(ActionCollections *q);
    void insertCollection(QmlActionCollection *collection);

    ActionCollections *q = nullptr;
    QMap<QString, QmlActionCollection *> m_collections;
};

#endif
