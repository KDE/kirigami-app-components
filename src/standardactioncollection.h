// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef STANDARDACTIONCOLLECTION_H
#define STANDARDACTIONCOLLECTION_H

#include <QObject>

#include "qmlactioncollection_p.h"

class StandardActionCollection : public QmlActionCollection
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString name READ name CONSTANT OVERRIDE)

public:
    StandardActionCollection(QObject *parent = nullptr);
    ~StandardActionCollection();
};

#endif
