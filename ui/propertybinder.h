/*
  propertybinder.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYBINDER_H
#define GAMMARAY_PROPERTYBINDER_H

#include "gammaray_ui_export.h"

#include <QMetaProperty>
#include <QObject>
#include <QPointer>
#include <QVector>

namespace GammaRay {
/** Helper class to bind two properties together, similar to QML. */
class GAMMARAY_UI_EXPORT PropertyBinder : public QObject
{
    Q_OBJECT
public:
    /** Creates a new PropertyBinder for syncing properties between @p source and @p destination.
     *  No properties are synchronized by default, use add() to change this.
     *  No initial synchronization is performed, called syncSourceToDestination() to change that.
     */
    explicit PropertyBinder(QObject *source, QObject *destination);

    /** Keeps @p sourceProp of @p source in sync with @p destProp of @p destination.
     *  At least the source property must have a change notification signal.
     *  This is a convenience overload for syncing a single property pair, initial synchronization
     *  from source to destination happens automatically.
     */
    explicit PropertyBinder(QObject *source, const char *sourceProp, QObject *destination,
                            const char *destProp);

    ~PropertyBinder() override;

    /** Adds another binding between @p sourceProp and @p destProp.
     *  At least the source property must have a change notification signal.
     */
    void add(const char *sourceProp, const char *destProp);

public slots:
    /** Use this for initial/explicit source to destination synchronization. */
    void syncSourceToDestination();

private slots:
    /** Same as the above, for the opposite direction. */
    void syncDestinationToSource();

private:
    QObject *m_source;
    QPointer<QObject> m_destination;
    struct Binding
    {
        QMetaProperty sourceProperty;
        QMetaProperty destinationProperty;
    };
    QVector<Binding> m_properties;
    bool m_lock;
};
}

#endif // GAMMARAY_PROPERTYBINDER_H
