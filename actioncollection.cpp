// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>


#include "actioncollection.h"


ActionCollectionData::ActionCollectionData(QObject *parent)
    : QObject(parent)
{}

ActionCollectionData::~ActionCollectionData()
{}

QString ActionCollectionData::collection() const
{
    return m_collection;
}

void ActionCollectionData::setCollection(const QString &collection)
{
    if (m_collection == collection)
        return;

    m_collection = collection;
    Q_EMIT collectionChanged();
}

