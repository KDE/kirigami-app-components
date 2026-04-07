// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2025 Marco Martin <notmart@gmail.com>

#include "actiondata_p.h"
#include "qmlactioncollection_p.h"

#include <QKeySequence>
#include <QQmlContext>
#include <QQmlProperty>
#include <QVariant>
#include <QtQml/qqmlinfo.h>

#include <KConfigGroup>
#include <KSharedConfig>

using namespace Qt::StringLiterals;

static QKeySequence variantToKeySequence(const QVariant &var)
{
    if (var.metaType().id() == QMetaType::Int) {
        return QKeySequence(static_cast<QKeySequence::StandardKey>(var.toInt()));
    }
    return QKeySequence::fromString(var.toString());
}

ActionGroup::ActionGroup(QObject *parent)
    : QActionGroup(parent)
{
}

class IconGroupPrivate
{
public:
    IconGroupPrivate(IconGroup *group)
        : q(group)
    {
    }

    IconGroup *q;
    ActionData *m_ad = nullptr;
    QString m_name;
    QString m_source;
    qreal m_width = -1;
    qreal m_height = -1;
    QColor m_color = Qt::transparent;
    bool m_cache = true;
};

IconGroup::IconGroup(ActionData *parent)
    : QObject(parent)
    , d(std::make_unique<IconGroupPrivate>(this))
{
    d->m_ad = parent;
}

IconGroup::~IconGroup()
{
}

QString IconGroup::name() const
{
    return d->m_name;
}

void IconGroup::setName(const QString &name)
{
    if (d->m_name == name) {
        return;
    }

    d->m_name = name;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.name"));
        property.write(name);
    }

    Q_EMIT nameChanged();
}

QString IconGroup::source() const
{
    return d->m_source;
}

void IconGroup::setSource(const QString &source)
{
    if (d->m_source != source) {
        return;
    }

    d->m_source = source;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.source"));
        property.write(source);
    }

    Q_EMIT sourceChanged();
}

qreal IconGroup::width() const
{
    return d->m_width;
}

void IconGroup::setWidth(qreal width)
{
    if (qFuzzyCompare(d->m_width, width)) {
        return;
    }

    d->m_width = width;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.width"));
        property.write(width);
    }

    Q_EMIT widthChanged();
}

qreal IconGroup::height() const
{
    return d->m_height;
}

void IconGroup::setHeight(qreal height)
{
    if (qFuzzyCompare(d->m_height, height)) {
        return;
    }

    d->m_height = height;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.height"));
        property.write(height);
    }

    Q_EMIT heightChanged();
}

QColor IconGroup::color() const
{
    return d->m_color;
}

void IconGroup::setColor(const QColor &color)
{
    if (d->m_color == color) {
        return;
    }

    d->m_color = color;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.color"));
        property.write(color);
    }

    Q_EMIT colorChanged();
}

bool IconGroup::cache() const
{
    return d->m_cache;
}

void IconGroup::setCache(bool cache)
{
    if (d->m_cache == cache) {
        return;
    }

    d->m_cache = cache;

    if (d->m_ad->action()) {
        QQmlProperty property(d->m_ad->action(), QStringLiteral("icon.cache"));
        property.write(cache);
    }

    Q_EMIT cacheChanged();
}

ActionData::ActionData(QObject *parent)
    : QAction(parent)
{
    m_icon = new IconGroup(this);
    connect(this, &ActionData::changed, this, &ActionData::syncDown);
    connect(this, &ActionData::checkableChanged, this, &ActionData::syncDown);
    connect(this, &ActionData::toggled, this, &ActionData::syncDown);
}

ActionData::~ActionData()
{
}

QString ActionData::name() const
{
    return m_name;
}

void ActionData::setName(const QString &name)
{
    if (m_name == name) {
        return;
    }

    m_name = name;

    Q_EMIT nameChanged(name);
}

IconGroup *ActionData::icon() const
{
    return m_icon;
}

QVariant ActionData::variantShortcut() const
{
    return shortcut();
}

void ActionData::setVariantShortcut(const QVariant &shortcut)
{
    // TODO: react to QAction::change
    const auto seq = shortcut.value<QKeySequence>();

    if (seq == QAction::shortcut()) {
        return;
    }

    auto scuts = shortcuts();
    if (scuts.isEmpty()) {
        setShortcut(seq);
    } else {
        scuts[0] = seq;
        setShortcuts(scuts);
    }

    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    cg = KConfigGroup(&cg, m_name);
    if (seq != m_defaultShortcut) {
        cg.writeEntry(QStringLiteral("Shortcut"), variantToKeySequence(shortcut).toString());
    } else {
        cg.deleteEntry(QStringLiteral("Shortcut"));
    }

    for (auto action : std::as_const(m_allActions)) {
        action->setProperty("shortcut", shortcut);
    }

    Q_EMIT shortcutChanged(shortcut);
}

QVariant ActionData::variantAlternateShortcut() const
{
    const auto &scuts = shortcuts();
    if (scuts.length() >= 2) {
        return scuts[1];
    }
    return QKeySequence();
}

void ActionData::setVariantAlternateShortcut(const QVariant &shortcut)
{
    const auto seq = shortcut.value<QKeySequence>();
    auto scuts = shortcuts();

    if (scuts.length() > 1 && scuts[1] == seq) {
        return;
    }

    if (scuts.isEmpty()) {
        scuts.append(QKeySequence());
    }
    if (scuts.length() < 2) {
        scuts.append(seq);
    } else {
        scuts[1] = seq;
    }

    setShortcuts(scuts);

    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    cg = KConfigGroup(&cg, m_name);
    if (shortcut != m_defaultAlternateShortcut) {
        cg.writeEntry(QStringLiteral("AlternateShortcut"), variantToKeySequence(shortcut).toString());
    } else {
        cg.deleteEntry(QStringLiteral("AlternateShortcut"));
    }

    Q_EMIT alternateShortcutChanged(shortcut);
}

QVariant ActionData::defaultShortcut() const
{
    return m_defaultShortcut;
}

void ActionData::setDefaultShortcut(const QVariant &shortcut)
{
    QKeySequence seq = variantToKeySequence(shortcut);
    if (m_defaultShortcut == seq) {
        return;
    }

    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    cg = KConfigGroup(&cg, m_name);
    QKeySequence savedShortcut = cg.readEntry(QStringLiteral("Shortcut"), m_defaultShortcut.toString());

    const bool wasDefault = savedShortcut == m_defaultShortcut;

    m_defaultShortcut = seq;

    if (wasDefault) {
        setVariantShortcut(m_defaultShortcut);
    }

    Q_EMIT defaultShortcutChanged(shortcut);
}

QVariant ActionData::defaultAlternateShortcut() const
{
    return m_defaultAlternateShortcut;
}

void ActionData::setDefaultAlternateShortcut(const QVariant &shortcut)
{
    if (m_defaultAlternateShortcut == shortcut) {
        return;
    }

    m_defaultAlternateShortcut = variantToKeySequence(shortcut);

    Q_EMIT defaultAlternateShortcutChanged(shortcut);
}

void ActionData::setActionGroupNotify(QActionGroup *group)
{
    setActionGroup(group);
    Q_EMIT actionGroupChanged(group);
}

QObject *ActionData::action() const
{
    return m_action;
}

void ActionData::setAction(QObject *action)
{
    if (m_action == action) {
        return;
    }

    if (m_action) {
        disconnect(m_action, nullptr, this, nullptr);
        disconnect(this, nullptr, m_action, nullptr);
        m_allActions.removeAll(m_action);
    }
    m_action = action;
    addKirigamiAction(action);

    Q_EMIT actionChanged(action);
}

QList<QObject *> ActionData::kirigamiActions() const
{
    return m_allActions;
}

void ActionData::addKirigamiAction(QObject *action)
{
    if (!action) {
        return;
    }

    connect(this, &ActionData::toggled, action, [this, action](bool checked) {
        action->setProperty("checked", checked);
    });
    connect(
        action,
        &QObject::destroyed,
        this,
        [this]() {
            m_allActions.removeAll(sender());
        },
        Qt::SingleShotConnection);

    if (!m_allActions.contains(action)) {
        m_allActions.append(action);
    }

    syncDown();
}

void ActionData::syncDown()
{
    for (auto action : std::as_const(m_allActions)) {
        QQmlProperty property(action, QStringLiteral("fromQAction"));
        if (property.isValid() && property.isWritable()) {
            property.write(QVariant::fromValue(this));
        } else {
            // It's a plain Templates action
            property = QQmlProperty(action, QStringLiteral("text"));
            property.write(text());
            property = QQmlProperty(action, QStringLiteral("icon.name"));
            property.write(m_icon->name());
            property = QQmlProperty(action, QStringLiteral("icon.source"));
            property.write(m_icon->source());
            property = QQmlProperty(action, QStringLiteral("shortcut"));
            property.write(shortcut());
            property = QQmlProperty(action, QStringLiteral("checkable"));
            property.write(isCheckable());
            property = QQmlProperty(action, QStringLiteral("checked"));
            property.write(isChecked());
        }
    }
}

void ActionData::classBegin()
{
    m_collection = qobject_cast<QmlActionCollection *>(parent());
    if (!m_collection) {
        QQmlError error;
        error.setDescription(QStringLiteral("ActionData must be a child of ActionCollection."));
        error.setMessageType(QtFatalMsg);
        QQmlContext *context = qmlContext(this);
        if (context) {
            error.setUrl(context->baseUrl());
        }
        qmlWarning(nullptr, error);
        qFatal();
    }
}

void ActionData::componentComplete()
{
    if (!m_collection) {
        return;
    }
    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Shortcuts"));
    cg = KConfigGroup(&cg, m_collection->name());
    cg = KConfigGroup(&cg, m_name);
    QKeySequence shortcut;
    if (cg.hasKey("Shortcut")) {
        shortcut = cg.readEntry(QStringLiteral("Shortcut"), QString());
    } else {
        shortcut = variantToKeySequence(m_defaultShortcut).toString();
    }

    QKeySequence alternateShortcut;
    if (cg.hasKey("AlternateShortcut")) {
        alternateShortcut = cg.readEntry(QStringLiteral("AlternateShortcut"), QString());
    } else {
        alternateShortcut = variantToKeySequence(m_defaultAlternateShortcut).toString();
    }

    setShortcuts({shortcut, alternateShortcut});

    if (!shortcuts().first().isEmpty()) {
        Q_EMIT shortcutChanged(shortcut);
    }

    if (!shortcuts().last().isEmpty()) {
        Q_EMIT alternateShortcutChanged(alternateShortcut);
    }

    syncDown();
}

#include "moc_actiondata_p.cpp"
