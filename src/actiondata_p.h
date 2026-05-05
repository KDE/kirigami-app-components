// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#pragma once

#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QQmlEngine>
#include <QVariant>
#include <qnamespace.h>
#include <qqmlregistration.h>

namespace KirigamiActions
{

class QmlActionCollection;
class ActionData;
class IconGroupPrivate;

class ActionDataGroup : public QActionGroup
{
    Q_OBJECT
    QML_ELEMENT
public:
    ActionDataGroup(QObject *parent = nullptr);
};

class IconGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)

public:
    IconGroup(ActionData *parent = nullptr);
    ~IconGroup();

    QString name() const;
    void setName(const QString &name);

    QString source() const;
    void setSource(const QString &source);

    qreal width() const;
    void setWidth(qreal width);

    qreal height() const;
    void setHeight(qreal height);

    QColor color() const;
    void setColor(const QColor &color);

    bool cache() const;
    void setCache(bool cache);

Q_SIGNALS:
    void nameChanged();
    void sourceChanged();
    void widthChanged();
    void heightChanged();
    void colorChanged();
    void cacheChanged();

private:
    std::unique_ptr<IconGroupPrivate> d;
};

/*!
 * \qmltype ActionData
 * \inqmlmodule org.kde.kirigami.actioncollection
 * \nativetype QAction
 *
 * \brief Declarative representation for a named action within the application
 * with user-configurable shortcuts
 *
 * This element needs to always be declared as a child of ActionCollection
 *
 * \code
 * import org.kde.kirigami.actioncollection as AC
 * ...
 * AC.ActionCollection {
 *     name: "EditActions"
 *     AC.ActionData {
 *        name: "exampleAction"
 *        icon.name: "edit-copy"
 *        text: "Copy"
 *     }
 *     ...
 * }
 * ...
 * Kirigami.Action {
 *   id: copyAction
 *   AC.ActionCollection.collection: "EditActions"
 *   AC.ActionCollection.action: "exampleAction"
 *   onTriggered: {
 *       ...
 *   }
 * }
 * \endcode
 */
class ActionData : public QAction, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT

    /*!
     * \qmlproperty string name
     *
     * The unique name of the action within its collection.
     * It is required and should be set only once
     *FIXME: this should be REQUIRED but not in StandardActionData
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)

    /*!
     * Description for the icon to use, which can be specified by name or path
     *
     * \qmlproperty string icon.name
     * \qmlproperty url icon.source
     * \qmlproperty int icon.width
     * \qmlproperty int icon.height
     * \qmlproperty color icon.color
     * \qmlproperty bool icon.cache
     */
    Q_PROPERTY(IconGroup *icon READ icon CONSTANT FINAL)

    /*!
     * \qmlproperty keysequence defaultShortcut

     * This property holds the action's default shortcut. The key sequence can be set
     * to one of the \l{QKeySequence::StandardKey}{standard keyboard shortcuts},
     * or it can be described with a string containing a sequence of up to four
     * key presses that are needed to trigger the shortcut.
     * This will be the shortcut of the action in the app, unless the user explicitly
     * confugured to use another one.

     * \code
     * ActionData {
     *     defaultShortcut: "Ctrl+E,Ctrl+W"
     * }
     * \endcode
     */
    Q_PROPERTY(QVariant defaultShortcut READ defaultShortcut WRITE setDefaultShortcut NOTIFY defaultShortcutChanged FINAL)

    /*!
     * \qmlproperty keysequence defaultAlternateShortcut
     *
     * This property holds the action's default alternate secondary shortcut.
     * The key sequence can be set
     * to one of the \l{QKeySequence::StandardKey}{standard keyboard shortcuts},
     * or it can be described with a string containing a sequence of up to four
     * key presses that are needed to trigger the shortcut.
     * This will be the shortcut of the action in the app, unless the user explicitly
     * configured to use another one.

     * \code
     * ActionData {
     *     defaultAlternateShortcut: "Ctrl+E,Ctrl+W"
     * }
     * \endcode
     */
    Q_PROPERTY(QVariant defaultAlternateShortcut READ defaultAlternateShortcut WRITE setDefaultAlternateShortcut NOTIFY defaultAlternateShortcutChanged FINAL)

    /*!
     * \qmlproperty action QtQuick.Templates::Action
     *
     * This property holds the QML Action this ActionData is associated to, if any.
     * Since ActionData is just a description of an action, in order to be an active
     * working action it has to be associated with a QML Action instance, preferable a
     * kirigami.Action specialization.
     *
     * It can be associated either here by binding this property to the id
     * of an Action instance or from the Action instance using the ActionGroup attached property.
     *
     * Direct example:
     * \code
     * ...
     * ActionCollection {
     *     ...
     *     ActionData {
     *        name: "exampleAction"
     *        icon.name: "edit-copy"
     *        text: "Copy"
     *        action: copyAction
     *     }
     *     ...
     * }
     * ...
     * Kirigami.Action {
     *   id: copyAction
     *   onTriggered: {
     *       ...
     *   }
     * }
     * \endcode
     *
     * Attached property example:
     * \code
     * ...
     * ActionCollection {
     *     name: "EditActions"
     *     ActionData {
     *        name: "exampleAction"
     *        icon.name: "edit-copy"
     *        text: "Copy"
     *     }
     *     ...
     * }
     * ...
     * Kirigami.Action {
     *   id: copyAction
     *   ActionCollection.collection: "EditActions"
     *   ActionCollection.action: "exampleAction"
     *   onTriggered: {
     *       ...
     *   }
     * }
     * \endcode
     */
    Q_PROPERTY(QObject *action READ action() WRITE setAction NOTIFY actionChanged)

    /*!
     * \qmlproperty QActionGroup actionGroup
     *TODO :perhaps this can be removed
     */
    Q_PROPERTY(QActionGroup *actionGroup READ actionGroup WRITE setActionGroupNotify NOTIFY actionGroupChanged)

    /*!
     * \qmlproperty QVariant data
     * Any extra data payload that can be put on the action
     */
    Q_PROPERTY(QVariant data READ data WRITE setData NOTIFY changed)

public:
    explicit ActionData(QObject *parent = nullptr);
    ~ActionData() override;

    QString name() const;
    void setName(const QString &name);

    IconGroup *icon() const;

    // TODO: all of thgose might hopefully be completely internal api
    QVariant variantShortcut() const;
    void setVariantShortcut(const QVariant &shortcut);

    QVariant variantAlternateShortcut() const;
    void setVariantAlternateShortcut(const QVariant &shortcut);

    QVariant defaultShortcut() const;
    void setDefaultShortcut(const QVariant &shortcut);

    QVariant defaultAlternateShortcut() const;
    void setDefaultAlternateShortcut(const QVariant &shortcut);

    void setActionGroupNotify(QActionGroup *group);

    QObject *action() const;
    void setAction(QObject *action);

    QList<QObject *> kirigamiActions() const;
    void addKirigamiAction(QObject *action);

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void iconChanged(const QString &icon);
    void shortcutChanged(const QVariant &shortcut);
    void alternateShortcutChanged(const QVariant &shortcut);
    void defaultShortcutChanged(const QVariant &defaultShortcut);
    void defaultAlternateShortcutChanged(const QVariant &defaultShortcut);
    void actionGroupChanged(QActionGroup *group);
    void actionChanged(QObject *action);

private:
    // TODO: dpointer
    void syncDown();
    IconGroup *m_icon;
    QString m_name;
    QKeySequence m_defaultShortcut;
    QKeySequence m_defaultAlternateShortcut;
    // Why both an m_action and an action list properties?
    // we want to support setting the action as it was a simple
    // property for the single case where the action collection and the
    // kirigami actions are in the same QML file, but we also want to be able to support multiple
    // action instances per action data, because we can have different pages
    // completely disconnetted from one to another that want to use that same action, like
    // a very common one such as "Copy"
    QPointer<QObject> m_action;
    QList<QObject *> m_allActions;
    QmlActionCollection *m_collection;
};

}
