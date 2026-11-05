// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "helper.h"

#include <QDebug>
#include <QQuickStyle>

Helper::Helper(QObject *parent)
    : QObject(parent)
{
}

QString Helper::styleName() const
{
    return QQuickStyle::name();
}

bool Helper::matches(const QString &pattern, const QString &text) const
{
    return text.contains(pattern, Qt::CaseInsensitive);
}
