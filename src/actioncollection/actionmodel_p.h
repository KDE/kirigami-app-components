// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#ifndef ACTIONMODEL_H
#define ACTIONMODEL_H

#include <QAbstractListModel>
#include <QQuickWindow>
#include <qqmlregistration.h>

#include "actiondata_p.h"

namespace KirigamiActions
{

class ActionModelPrivate;

class ActionModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActionModel)
    // TODO: this should be able to have multiple collections? or just a KConcatenateProxyModel?
    Q_PROPERTY(QString collectionName READ collectionName WRITE setCollectionName NOTIFY collectionNameChanged FINAL)
    Q_PROPERTY(ShownActions shownActions READ shownActions WRITE setShownActions NOTIFY shownActionsChanged FINAL)
    /*!
     * \qmlproperty Window ActionModel::window
     *
     * Limits each action's instances to this window. A null window includes
     * instances from all windows. Action definitions and shortcuts remain shared.
     */
    Q_PROPERTY(QQuickWindow *window READ window WRITE setWindow NOTIFY windowChanged FINAL)

public:
    enum ShownActions {
        AllActions = 0,
        ActiveActions
    };
    Q_ENUM(ShownActions);

    enum Role {
        ActionDescriptionRole = Qt::UserRole + 1,
        ActionInstancesRole,
        ActionCollectionRole,
        ShortcutRole,
        AlternateShortcutRole,
    };
    Q_ENUM(Role);

    explicit ActionModel(QObject *parent = nullptr);
    ~ActionModel() override;

    QString collectionName() const;
    void setCollectionName(const QString &name);

    ShownActions shownActions() const;
    void setShownActions(ShownActions shown);

    QQuickWindow *window() const;
    void setWindow(QQuickWindow *window);

    Q_INVOKABLE void save();
    Q_INVOKABLE void reset(const QModelIndex &index);
    Q_INVOKABLE void resetAll();

    QmlActionCollection *collection() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void windowChanged();
    void collectionNameChanged(const QString &name);
    void shownActionsChanged(KirigamiActions::ActionModel::ShownActions shown);

private:
    ActionModelPrivate *d = nullptr;
    friend class ActionModelPrivate;
};

}

#endif
