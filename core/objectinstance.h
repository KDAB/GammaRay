/*
  objectinstance.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSTANCE_H
#define GAMMARAY_OBJECTINSTANCE_H

#include "gammaray_core_export.h"

#include <QByteArray>
#include <QPointer>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QObject;
struct QMetaObject;
QT_END_NAMESPACE

namespace GammaRay {
/** Represents some form of object the property adaptor/model code can handle. */
class GAMMARAY_CORE_EXPORT ObjectInstance
{
public:
    enum Type
    {
        Invalid,
        QtObject,
        QtMetaObject,
        QtGadgetPointer, ///< a pointer to a gadget
        QtVariant,
        Object,
        Value,
        QtGadgetValue ///< a gadget value type stored in the QVariant
    };
    ObjectInstance() = default;
    ObjectInstance(QObject *obj); // krazy:exclude=explicit
    /// use this for Q_GADGETs
    ObjectInstance(void *obj, const QMetaObject *metaObj);
    /// use for things that only exist as GammaRay meta objects
    ObjectInstance(void *obj, const char *typeName);
    ObjectInstance(const QVariant &value); // krazy:exclude=explicit
    ObjectInstance(const ObjectInstance &other);

    ObjectInstance &operator=(const ObjectInstance &other);
    bool operator==(const ObjectInstance &rhs) const;

    Type type() const;
    /**!
     * Returns @c true if this instance holds a value type.
     * This is important to treat differently when writing to it, as this will be a copy
     * of the value you want to change.
     */
    bool isValueType() const;

    /// only valid for QtObject
    QObject *qtObject() const;
    /// only valid for QtObject, QtGadget and Object
    void *object() const;
    /// only valid for QtVariant
    const QVariant &variant() const;

    /// only valid for QtObject and QtGadget
    const QMetaObject *metaObject() const;
    /// only valid for [Qt]Object and QtGadget
    QByteArray typeName() const;

    /// Returns @c false if this instance is known to be invalid.
    bool isValid() const;

private:
    void copy(const ObjectInstance &other);
    void unpackVariant();

    void *m_obj = nullptr;
    QPointer<QObject> m_qtObj;
    QVariant m_variant;
    const QMetaObject *m_metaObj = nullptr;
    QByteArray m_typeName;
    Type m_type = Invalid;
};
}

#endif // GAMMARAY_OBJECTINSTANCE_H
