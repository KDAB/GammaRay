/*
  qtivipropertyoverrider.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "qtivipropertyoverrider.h"

#include <QObject>
#include <QVarLengthArray>
#include <QMetaEnum>

#include <QIviProperty>
#include <private/qiviproperty_p.h>

using namespace GammaRay;

namespace GammaRay {

/*
 For the purpose of overriding, there is a big difference between read-only and read-write properties.
 Read-write properties are effectively settings for lower layers from higher layers, with reading them
 back as a convenience function.
 Read-only properties are information about lower layers for higher layers.
 We handle them as follows:
 - Read-write properties:
   - Non-override mode: set them like user input would
   - Override mode: set them and make them return the set value, but don't pass it along to the
     backend (e.g. when setting the target temperature of air conditioning, only the target
     temperature as displayed would change, but the air in the car wouldn't be regulated warmer
     or colder).
 - Read-only properties:
   - Non-override mode: does not change any value
   - Override mode: As with read-write properties, only that the backend value couldn't
     be changed anyway (e.g. outside temperature).
 */

// Note: Those subclasses of QSlotObjectBase are not invoked through the ReadProperty / WriteProperty
// "opcodes" of the metaobject system, they are just regular methods, for which the calling convention
// is that a[0] points to the return value, a[1] to the first argument, a[2] to the second etc.

class OverrideValueSetter : public QtPrivate::QSlotObjectBase
{
    static void impl(int which, QSlotObjectBase *this_, QObject *, void **a, bool *ret)
    {
        switch (which) {
        case Destroy:
            delete static_cast<OverrideValueSetter*>(this_);
            break;
        case Call: {
            // store the value for later when we restore the original getters and setters - then we
            // can also restore the last value received from the real backend.
            OverrideValueSetter *const self = static_cast<OverrideValueSetter*>(this_);
            const QMetaType::Type mt = static_cast<QMetaType::Type>(self->m_stashedValue.userType());
            void *const firstArgument = a[1];
            self->m_stashedValue = QVariant(mt, firstArgument);
            break;
        }
        case Compare:
            *ret = false; // not implemented
            break;
        case NumOperations: ;
        }
    }
public:
    QVariant m_stashedValue;
    explicit OverrideValueSetter(const QVariant &value)
        : QSlotObjectBase(&impl), m_stashedValue(value) {}
};

class OverrideValueGetter : public QtPrivate::QSlotObjectBase
{
    static void impl(int which, QSlotObjectBase *this_, QObject *, void **a, bool *ret)
    {
        switch (which) {
        case Destroy:
            delete static_cast<OverrideValueGetter*>(this_);
            break;
        case Call: {
            QtIviPropertyOverrider *const overrider = static_cast<OverrideValueGetter*>(this_)->m_overrider;
            const QMetaType::Type mt = static_cast<QMetaType::Type>(overrider->m_overrideValue.userType());
            void *const returnValue = a[0];
            // There is no QMetaType::assign(), but we can still avoid re-allocating the storage.
            // No reallocation is a hard requirement in case the instance is not heap-allocated.
            QMetaType::destruct(mt, returnValue);
            QMetaType::construct(mt, returnValue, overrider->m_overrideValue.constData());
            break;
        }
        case Compare:
            *ret = false; // not implemented
            break;
        case NumOperations: ;
        }
    }
public:
    QtIviPropertyOverrider *m_overrider;
    explicit OverrideValueGetter(QtIviPropertyOverrider *overrider)
        : QSlotObjectBase(&impl), m_overrider(overrider) {}
};

}

QtIviPropertyOverrider::QtIviPropertyOverrider(QIviProperty *overriddenProperty)
   : m_prop(overriddenProperty)
{
}

QtIviPropertyOverrider::QtIviPropertyOverrider(QtIviPropertyOverrider &&other)
{
    *this = std::move(other);
}

QtIviPropertyOverrider &QtIviPropertyOverrider::operator=(QtIviPropertyOverrider &&other)
{
    m_prop = other.m_prop;
    m_overrideValue = other.m_overrideValue;
    m_originalValueGetter = other.m_originalValueGetter;
    m_originalValueSetter = other.m_originalValueSetter;

    if (isOverride()) {
        // fix up the backlink
        QIviPropertyPrivate *pPriv = QIviPropertyPrivate::get(m_prop);
        static_cast<OverrideValueGetter *>(pPriv->m_valueGetter)->m_overrider = this;
    }
    return *this;
}

QtIviPropertyOverrider::~QtIviPropertyOverrider()
{
    if (m_prop) {
        setOverride(false);
    }
}

void QtIviPropertyOverrider::setValue(const QVariant &value)
{
    if (isOverride()) {
        m_overrideValue = value;
    } else {
        if (!userWritable()) {
            qWarning("QtIviPropertyOverrider::setValue(): cannot set value on "
                     "non-writable non overridden property.");
            return;
        }
        m_prop->setValue(value);
    }
}

void QtIviPropertyOverrider::setOverride(bool doOverride)
{
    Q_ASSERT(m_prop);
    QIviPropertyPrivate *const pPriv = QIviPropertyPrivate::get(m_prop);
    const bool wasOverride = isOverride();
    if (!doOverride && wasOverride) {
        pPriv->m_valueGetter->destroyIfLastRef();
        pPriv->m_valueGetter = m_originalValueGetter;
        m_originalValueGetter = nullptr;
        if (pPriv->m_valueSetter) { // if it was nullptr, we left it nullptr
            // restore stashed value from the fake setter we injected
            const QVariant value = static_cast<OverrideValueSetter *>(pPriv->m_valueSetter)->m_stashedValue;
            pPriv->m_valueSetter->destroyIfLastRef();
            pPriv->m_valueSetter = m_originalValueSetter;
            m_originalValueSetter = nullptr;
            m_prop->setValue(value);
        }
    } else if (doOverride && !wasOverride) {
        const QVariant value = m_prop->value();
        m_originalValueGetter = pPriv->m_valueGetter;
        pPriv->m_valueGetter = new OverrideValueGetter(this);
        if (pPriv->m_valueSetter) { // if it is nullptr (-> read-only property), leave it nullptr
            m_originalValueSetter = pPriv->m_valueSetter;
            pPriv->m_valueSetter = new OverrideValueSetter(value);
        }
    }
    Q_ASSERT(isOverride() == doOverride);
}

bool QtIviPropertyOverrider::isOverride() const
{
    return m_originalValueGetter;
}

bool QtIviPropertyOverrider::userWritable() const
{
    return QIviPropertyPrivate::get(m_prop)->m_valueSetter;
}

QVariant QtIviPropertyOverrider::cppValue() const
{
    // QIviProperty::value() unfortunately "sanitizes" the type to avoid confusing the QML engine with
    // C++ types like enums and flags. We prefer the real types so we can show suitable display and
    // editing delegates for them. So reimplement QIviProperty[Factory]::value() without the type munging.
    QIviPropertyPrivate *const pPriv = QIviPropertyPrivate::get(m_prop);

    // use quint64 to get a healthy alignment - hopefully it is enough!
    QVarLengthArray<quint64, 2> storage;
    const int typeSize = QMetaType::sizeOf(pPriv->m_type);
    storage.resize((typeSize + sizeof(quint64) - 1) / sizeof(quint64));

    void *const rawStorage = reinterpret_cast<void *>(storage.data());
    void *args[] = { rawStorage, nullptr };
    pPriv->m_valueGetter->call(m_prop->parent(), args);

    return QVariant(pPriv->m_type, rawStorage);
}
