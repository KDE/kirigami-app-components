// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#pragma once

#include <QAction>

class ActionCollection;
class ActionDataPrivate;

// Accessible from both C++ and QML
class ActionData : public QAction
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

public:
    ~ActionData() override;

    QString name() const;
    void setName(const QString &name);

    ActionCollection *collection() const;

Q_SIGNALS:
    void nameChanged(const QString &name);

protected:
    explicit ActionData(QObject *parent = nullptr);

private:
    ActionDataPrivate *const d;
};
