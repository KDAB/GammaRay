/*
  objectinstance.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_OBJECTINSTANCE_H
#define GAMMARAY_OBJECTINSTANCE_H

#include "gammaray_core_export.h"

#include <QByteArray>
#include <QVariant>

class QObject;
struct QMetaObject;

namespace GammaRay {

/** Represents some form of object the property adaptor/model code can handle. */
class GAMMARAY_CORE_EXPORT ObjectInstance
{
public:
    enum Type {
        Invalid,
        QtObject,
        QtGadget,
        QtVariant,
        Object,
    };
    ObjectInstance();
    ObjectInstance(QObject *obj);
    /// use this for Q_GADGETs
    ObjectInstance(void *obj, const QMetaObject *metaObj);
    /// use for things that only exist as GammaRay meta objects
    ObjectInstance(void *obj, const char* typeName);
    ObjectInstance(const QVariant &value);

    Type type() const;

    /// only valid for QtObject
    QObject* qtObject() const;
    /// only valid for QtObject, QtGadget and Object
    void* object() const;
    /// only valid for QtVariant
    QVariant variant() const;

    /// only valid for QtObject and QtGadget
    const QMetaObject* metaObject() const;
    /// only valid for [Qt]Object and QtGadget
    QByteArray typeName() const;

private:
    union {
        void *obj;
        QObject *qtObj;
    } m_payload;
    QVariant m_variant;
    const QMetaObject *m_metaObj;
    QByteArray m_typeName;
    Type m_type;
};

}

#endif // GAMMARAY_OBJECTINSTANCE_H
