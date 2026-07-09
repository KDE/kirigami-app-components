// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Marco Martin <notmart@gmail.com>

#pragma once

#include "actiondata_p.h"
#include <KStandardActions>

namespace KirigamiActions
{
/*!
 * \qmltype StandardActionData
 * \inqmlmodule org.kde.kirigami.actioncollection
 *
 * \brief Declarative representation for a named action derivarive from
 * KStandardActions.
 *
 * This element needs to always be declared as a child of ActionCollection
 *
 * \code
 * import org.kde.kirigami.actioncollection as AC
 * ...
 * AC.ActionCollection {
 *     name: "EditActions"
 *     AC.StandardActionData {
 *        standardAction: AC.StandardActionData.Copy
 *     }
 *     ...
 * }
 * ...
 * Kirigami.Action {
 *   id: copyAction
 *   AC.ActionCollection.collection: "EditActions"
 *   AC.ActionCollection.action: AC.StandardActionData.Copy
 *   onTriggered: {
 *       ...
 *   }
 * }
 * \endcode
 */
class StandardActionData : public ActionData
{
    Q_OBJECT
    QML_ELEMENT
    QML_EXTENDED_NAMESPACE(KStandardActions)

    /*!
     * \qmlproperty StandardAction StandardActionData::standardAction
     * The standard action this StandardActionData rapresents
     */
    Q_PROPERTY(KStandardActions::StandardAction standardAction READ standardAction WRITE setStandardAction NOTIFY standardActionChanged FINAL REQUIRED)

public:
    explicit StandardActionData(QObject *parent = nullptr);
    ~StandardActionData() override;

    KStandardActions::StandardAction standardAction() const;
    void setStandardAction(KStandardActions::StandardAction standardAction);

Q_SIGNALS:
    void standardActionChanged(KStandardActions::StandardAction standardAction);

private:
    KStandardActions::StandardAction m_standardAction = KStandardActions::ActionNone;
};

}
