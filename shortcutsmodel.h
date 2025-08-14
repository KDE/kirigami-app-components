// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include "actioncollection.h"

#include <QAbstractListModel>
#include <QObject>
#include <qqmlregistration.h>
#include <QQmlEngine>

class ShortcutsModel : public QAbstractListModel {
    Q_OBJECT
    QML_NAMED_ELEMENT(ShortcutsModel)
    // TODO: this should be able to have multiple names and multiple collections
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

public:
    enum Role {
        ActionNameRole = Qt::UserRole + 1,
        IconNameRole,
        ShortcutRole,
        ShortcutDisplayRole,
        DefaultShortcutRole,
        AlternateShortcutsRole,
        CollectionNameRole,
    };

    explicit ShortcutsModel(QObject *parent = nullptr);
    ~ShortcutsModel() override;

    QString name() const;
    void setName(const QString &name);

    // Somewhere outside ShortcutsModel ?
    Q_INVOKABLE void setShortcut(const QString &name, const QString &shortcut);

    ActionCollection *collection() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void nameChanged(const QString &name);

private:
    QPointer<ActionCollection> m_collection;
};
