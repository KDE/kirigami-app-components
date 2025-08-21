// SPDX-License-Identifier: LGPL-2.0-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include <QObject>
#include <qqmlregistration.h>
#include <QQmlEngine>
#include <QVariant>

#include <QKeySequence>

class ActionCollection;

// Accessible from both C++ and QML
class ActionData : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_NAMED_ELEMENT(ActionData)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged FINAL)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged FINAL)
    Q_PROPERTY(QVariant defaultShortcut READ defaultShortcut WRITE setDefaultShortcut NOTIFY defaultShortcutChanged FINAL)
    Q_PROPERTY(QVariant shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged FINAL)
    // TODO: checkable? other Action properties to proxy? proxy trigger and triggered? at this poing might well be a QAction?
public:
    explicit ActionData(QObject *parent = nullptr);
    ~ActionData() override;

    QString name() const;
    void setName(const QString &name);

    QString text() const;
    void setText(const QString &text);

    //TODO: replace with the usual grouped property
    QString icon() const;
    void setIcon(const QString &name);

    QVariant shortcut() const;
    void setShortcut(const QVariant &shortcut);

    QVariant defaultShortcut() const;
    void setDefaultShortcut(const QVariant &shortcut);

    QObject *action() const;
    void setAction(QObject *action);

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void textChanged(const QString &text);
    void iconChanged(const QString &icon);
    void shortcutChanged(const QVariant &shortcut);
    void defaultShortcutChanged(const QVariant &defaultShortcut);
    void actionChanged(QObject *action);

private:
    // TODO: dpointer
    void syncProperties();
    QString m_name;
    QString m_text;
    QString m_icon;
    QVariant m_shortcut;
    QVariant m_defaultShortcut;
    QPointer<QObject> m_action;
    ActionCollection *m_collection;
};

