/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *  SPDX-FileCopyrightText: 2026 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <quicktest.h>

#include <KLocalizedQmlContext>
#include <QQmlEngine>

class KirigamiAppComponentsSetup : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        // The KI18n singleton needs the engine's localized context
        KLocalization::setupLocalizedContext(engine);
    }
};

QUICK_TEST_MAIN_WITH_SETUP(KirigamiAppComponents, KirigamiAppComponentsSetup)

#include "qmltest.moc"
