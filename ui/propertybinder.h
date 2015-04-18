/*
  propertybinder.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_PROPERTYBINDER_H
#define GAMMARAY_PROPERTYBINDER_H

#include "gammaray_ui_export.h"

#include <QMetaProperty>
#include <QObject>
#include <QPointer>

namespace GammaRay {

/** Helper class to bind two properties together, similar to QML. */
class GAMMARAY_UI_EXPORT PropertyBinder : public QObject
{
    Q_OBJECT
public:
    /** Keeps @p sourceProp of @p source in sync with @p destProp of @p destination.
     *  At least one property must have a change notification signal.
     */
    explicit PropertyBinder(QObject *source, const char *sourceProp, QObject *destination, const char *destProp);
    ~PropertyBinder();

private slots:
    void sourceChanged();
    void destinationChanged();

private:
    QObject* m_source;
    QPointer<QObject> m_destination;
    QMetaProperty m_sourceProperty;
    QMetaProperty m_destinationProperty;
    bool m_lock;
};
}

#endif // GAMMARAY_PROPERTYBINDER_H
