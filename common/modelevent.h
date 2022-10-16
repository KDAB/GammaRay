/*
  modelevent.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MODELEVENT_H
#define GAMMARAY_MODELEVENT_H

#include "gammaray_common_export.h"

#include <QEvent>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
QT_END_NAMESPACE

namespace GammaRay {
/** Custom event to notify models about them being used/unused.
 *  Useful for suspending expensive operations.
 */
class GAMMARAY_COMMON_EXPORT ModelEvent : public QEvent
{
public:
    explicit ModelEvent(bool modelUsed);
    ~ModelEvent();

    /** Returns @c true if somebody is using the model. */
    bool used() const;

    /** Returns the event id for this type. */
    static QEvent::Type eventType();

private:
    Q_DISABLE_COPY(ModelEvent)
    bool m_used;
};

namespace Model {
/** Convenience function to send a ModelEvent indicating @p model is in use. */
void GAMMARAY_COMMON_EXPORT used(const QAbstractItemModel *model);
/** Convenience function to send a ModelEvent indicating nobody is using @p model. */
void GAMMARAY_COMMON_EXPORT unused(QAbstractItemModel *model);
}
}

#endif // GAMMARAY_MODELEVENT_H
