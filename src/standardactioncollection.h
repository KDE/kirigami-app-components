// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef STANDARDACTIONCOLLECTION_H
#define STANDARDACTIONCOLLECTION_H

#include <QObject>

#include "actioncollection.h"

class StandardActionCollection : public ActionCollection
{
    Q_OBJECT
    QML_ELEMENT

public:
    StandardActionCollection(QObject *parent = nullptr);
    ~StandardActionCollection();
};

#endif
