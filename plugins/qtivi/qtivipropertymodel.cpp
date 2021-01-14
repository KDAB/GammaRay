/*
  qtivipropertymodel.cpp

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

#include "qtivipropertymodel.h"

#include <core/enumrepositoryserver.h>
#include <core/enumutil.h>
#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>
#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/objectmodel.h>

#include <QIviServiceObject>
#include <QIviAbstractZonedFeature>
#if defined(QT_IVIMEDIA_LIB)
#include <QIviAmFmTuner>
#endif
#include <QIviProperty>
#include <private/qiviproperty_p.h>

#include <QThread>
#include <QItemSelectionModel>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMutexLocker>

#include <iostream>

static const quint64 PropertyCarrierIndex = ~quint64(0); // there is no QUINT64_MAX

//#define IF_DEBUG(x) (x)
#define IF_DEBUG(x)

using namespace GammaRay;

// Note: convertToCppType() could be used instead of or to implement QtIviPropertyOverrider::cppValue() -
// the latter just seems more direct and less likely to go wrong for now.

// Convert back to C++ type if it was sanitized for QML - trying to imitate what
// QIviProperty::setValue(const QVariant &value) does; note that the interesting
// part of setValue() is the int * -> void * conversion for the args array.
// The rest of setValue() checks to see if it is okay to do that :)
static QVariant convertToCppType(QVariant qmlVar, int cppTypeId)
{
    if (qmlVar.userType() != cppTypeId) {
        if (qmlVar.canConvert(cppTypeId)) {
            qmlVar.convert(cppTypeId);
        } else {
            const QMetaEnum me = EnumUtil::metaEnum(QVariant(cppTypeId, nullptr));
            if (me.isValid() && qmlVar.canConvert<int>()) {
                const int rawValue = qmlVar.toInt();
                // An int is not "naturally" convertible to an enum or flags unless
                // a conversion is explicitly registered.
                // ### If a Q_ENUM could inherit long long, this could be an uninitialized memory read.
                // I think it can't be long long, due to QMetaEnum API being int-based.
                return QVariant(cppTypeId, &rawValue);
            }
        }
    }
    return qmlVar;
}

QtIviPropertyModel::QtIviPropertyModel(Probe *probe)
    : QAbstractItemModel(probe)
{
    // Known wrongly qualified types
    qRegisterMetaType<QIviAbstractFeature::DiscoveryMode>();
    qRegisterMetaType<QIviAbstractFeature::DiscoveryResult>();
#if defined(QT_IVIMEDIA_LIB)
    qRegisterMetaType<QIviAmFmTuner::Band>();
#endif

    connect(probe, SIGNAL(objectCreated(QObject*)), this, SLOT(objectAdded(QObject*)));
    connect(probe, SIGNAL(objectDestroyed(QObject*)), this, SLOT(objectRemoved(QObject*)));
    connect(probe, SIGNAL(objectReparented(QObject*)), this, SLOT(objectReparented(QObject*)));
    connect(probe, SIGNAL(objectSelected(QObject*,QPoint)), this, SLOT(objectSelected(QObject*)));
}

QtIviPropertyModel::IviCarrierProperty::IviCarrierProperty(QtIviPropertyModel::IviCarrierProperty &&other)
{
    *this = std::move(other);
}

QtIviPropertyModel::IviCarrierProperty::IviCarrierProperty(QIviProperty *iviProperty, const QMetaProperty &metaProperty)
    : m_metaProperty(metaProperty)
    , m_iviProperty(iviProperty)
    , m_iviOverrider(iviProperty)
{
}

QtIviPropertyModel::IviCarrierProperty::IviCarrierProperty(const QMetaProperty &metaProperty)
    : m_metaProperty(metaProperty)
    , m_iviProperty(nullptr)
{
}

bool QtIviPropertyModel::IviCarrierProperty::isValid() const
{
    return m_metaProperty.isValid();
}

bool QtIviPropertyModel::IviCarrierProperty::isAvailable() const
{
    return m_iviProperty ? m_iviProperty->isAvailable() : m_metaProperty.isValid();
}

bool QtIviPropertyModel::IviCarrierProperty::hasNotifySignal() const
{
    return m_iviProperty ? true : m_metaProperty.hasNotifySignal();
}

bool QtIviPropertyModel::IviCarrierProperty::isWritable() const
{
    return (m_iviProperty
            ? m_iviOverrider.userWritable()
            : m_metaProperty.isWritable() &&
              !QMetaType(m_metaProperty.userType()).flags().testFlag(QMetaType::PointerToQObject));
}

bool QtIviPropertyModel::IviCarrierProperty::isOverridable() const
{
    return m_iviProperty;
}

bool QtIviPropertyModel::IviCarrierProperty::isOverrided() const
{
    return m_iviProperty && m_iviOverrider.isOverride();
}

QString QtIviPropertyModel::IviCarrierProperty::name() const
{
    return QString::fromUtf8(m_metaProperty.name());
}

QString QtIviPropertyModel::IviCarrierProperty::typeName() const
{
    const int userType(m_iviProperty ? m_iviOverrider.cppValue().userType() : m_metaProperty.userType());
    return QString::fromLatin1(QMetaType::typeName(userType));
}

ObjectId QtIviPropertyModel::IviCarrierProperty::objectId() const
{
    return ObjectId(m_iviProperty);
}

QString QtIviPropertyModel::IviCarrierProperty::displayText(const QObject *carrier) const
{
    const QVariant value(cppValue(carrier));
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QString enumStr(EnumUtil::enumToString(value, nullptr, mo));
    if (!enumStr.isEmpty())
        return enumStr;
    return VariantHandler::displayString(value);
}

QVariant QtIviPropertyModel::IviCarrierProperty::editValue(const QObject *carrier) const
{
    const QVariant value(cppValue(carrier));
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QMetaEnum me(EnumUtil::metaEnum(value, nullptr, mo));
    if (me.isValid()) {
        const int num(EnumUtil::enumToInt(value, me));
        return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
    }
    return VariantHandler::serializableVariant(value);
}

QVariant QtIviPropertyModel::IviCarrierProperty::cppValue(const QObject *carrier) const
{
    return m_iviProperty ? m_iviOverrider.cppValue() : m_metaProperty.read(carrier);
}

QVariant QtIviPropertyModel::IviCarrierProperty::iviConstraints() const
{
    if (!m_iviProperty)
        return QVariant();

    // value range?
    const QVariant min = m_iviProperty->minimumValue();
    const QVariant max = m_iviProperty->maximumValue();
    if (min != max)
        return QVariantList() << quint32(QtIviPropertyModel::RangeConstraints) << min << max;

    // list of allowed values?
    const QVariantList rawAvail = m_iviProperty->availableValues();
    if (!rawAvail.isEmpty()) {
        QVariantList avail;
        avail << uint(QtIviPropertyModel::AvailableValuesConstraints);

        for (const QVariant &qmlVar : rawAvail) {
            const QVariant v = convertToCppType(qmlVar, QIviPropertyPrivate::get(m_iviProperty)->m_type);

            // For convenience on the view side, send for each allowed value:
            // 1. Display string to display to the user
            // 2. "Raw" value for passing to setData() after editing
            const QMetaEnum me = EnumUtil::metaEnum(v);
            if (me.isValid()) {
                const int num = EnumUtil::enumToInt(v, me);
                if (me.isFlag())
                    avail << QString::fromLatin1(me.valueToKeys(num));
                else
                    avail << QString::fromLatin1(me.valueToKey(num));
                avail << QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
            } else {
                avail << VariantHandler::displayString(v);
                avail << VariantHandler::serializableVariant(v);
            }
        }
        return avail;
    }
    return QVariant();
}

void QtIviPropertyModel::IviCarrierProperty::setOverrided(bool override)
{
    if (override != m_iviOverrider.isOverride()) {
        if (override) {
            // for the initial override value, keep the "real" value from original backend
            const QVariant originalValue = m_iviOverrider.cppValue();
            m_iviOverrider.setOverride(true);
            m_iviOverrider.setValue(originalValue);
        } else {
            // revert to value from original backend
            m_iviOverrider.setOverride(false);
            emit m_iviProperty->valueChanged(m_iviProperty->value());
        }
    }
}

bool QtIviPropertyModel::IviCarrierProperty::setValue(const QVariant &value, QObject *carrier)
{
    Q_ASSERT(isAvailable());

    QVariant toSet = value;
    if (value.userType() == qMetaTypeId<EnumValue>()) {
        QVariant typeReference = cppValue(carrier);
        if (typeReference.type() == QVariant::Int) {
            toSet = value.value<EnumValue>().value();
        } else {
            *(static_cast<int*>(typeReference.data())) = value.value<EnumValue>().value();
            toSet = typeReference;
        }
    }

    if (m_iviProperty) {
        const bool wasOverride = isOverrided();
        bool isOverride = wasOverride || !isWritable();

        if (isOverride && !wasOverride && isOverridable()) {
            setOverrided(isOverride);
        }

        m_iviOverrider.setValue(toSet);

        // Hack: some properties reject value changes with no general way to know that
        // up front, so check and compensate similarly to "proper" read-only properties.
        if (!isOverride && m_iviOverrider.cppValue() != toSet) {
            isOverride = true;
            setOverrided(isOverride);
            m_iviOverrider.setValue(toSet);
        }

        // Make sure the model can update itself
        if (isOverride) {
            emit m_iviProperty->valueChanged(m_iviProperty->value());
        } // ... else the valueChanged() signal was hopefully emitted from the setter

        return true;
    }
    else {
        if (!m_metaProperty.isWritable()) {
            return false;
        }

        return m_metaProperty.write(carrier, toSet);
    }
}

QtIviPropertyModel::IviCarrierProperty &QtIviPropertyModel::IviCarrierProperty::operator=(QtIviPropertyModel::IviCarrierProperty &&other)
{
    m_metaProperty = other.m_metaProperty;
    m_iviProperty = other.m_iviProperty;
    m_iviOverrider = std::move(other.m_iviOverrider);
    return *this;
}

bool QtIviPropertyModel::IviCarrierProperty::operator==(const QIviProperty *property) const
{
    return this->m_iviProperty == property;
}

bool QtIviPropertyModel::IviCarrierProperty::operator==(const QByteArray &property) const
{
    return this->m_metaProperty.name() == property;
}

QtIviPropertyModel::IviCarrier::IviCarrier(QObject *carrier)
    : m_carrier(carrier)
{
}

QString QtIviPropertyModel::IviCarrier::label() const
{
    QString name = m_carrier->objectName();
    if (name.isEmpty()) {
        if (const QIviAbstractZonedFeature *zoned = qobject_cast<const QIviAbstractZonedFeature *>(m_carrier)) {
            // not translated; the zone API is fixed to English, too
            name = QString::fromLatin1("Zone: %1").arg(zoned->zone());
        }
    }
    if (name.isEmpty()) {
        name = QString::fromLatin1("%1(0x%2)")
                .arg(typeName())
                .arg(QString::number(quintptr(m_carrier), 16));
    }
    return name;
}

QString QtIviPropertyModel::IviCarrier::typeName() const
{
    return QString::fromLatin1(m_carrier->metaObject()->className());
}

ObjectId QtIviPropertyModel::IviCarrier::objectId() const
{
    return ObjectId(m_carrier);
}

int QtIviPropertyModel::IviCarrier::propertyCount() const
{
    return m_properties.size();
}

void QtIviPropertyModel::IviCarrier::pushProperty(IviCarrierProperty &&property)
{
    m_properties.push_back(std::move(property));
}

const QtIviPropertyModel::IviCarrierProperty &QtIviPropertyModel::IviCarrier::propertyAt(int index) const
{
    return m_properties.at(index);
}

QtIviPropertyModel::IviCarrierProperty &QtIviPropertyModel::IviCarrier::propertyAt(int index)
{
    return m_properties.at(index);
}

int QtIviPropertyModel::IviCarrier::indexOfProperty(const QIviProperty *property) const
{
    for (uint i = 0; i < m_properties.size(); i++) {
        if (m_properties.at(i) == property) {
            return i;
        }
    }
    return -1;
}

int QtIviPropertyModel::IviCarrier::indexOfProperty(const QByteArray &property) const
{
    for (uint i = 0; i < m_properties.size(); i++) {
        if (m_properties.at(i) == property) {
            return i;
        }
    }
    return -1;
}

bool QtIviPropertyModel::IviCarrier::operator==(const QObject *carrier) const
{
    return m_carrier == carrier;
}

int QtIviPropertyModel::rowOfCarrier(const QObject *carrier) const
{
    for (uint i = 0; i < m_carriers.size(); i++) {
        if (m_carriers.at(i) == carrier) {
            return i;
        }
    }
    return -1;
}

QModelIndex QtIviPropertyModel::indexOfCarrier(const QObject *carrier, int column) const
{
    const int row(rowOfCarrier(carrier));
    return row == -1 ? QModelIndex() : index(row, column);
}

QModelIndex QtIviPropertyModel::indexOfProperty(const QIviProperty *property, int column) const
{
    Q_ASSERT(property->parent());
    const int carrierRow = rowOfCarrier(property->parent());
    if (carrierRow < 0)
        return QModelIndex();
    const IviCarrier &carrier = m_carriers.at(carrierRow);
    const int propertyIndex = carrier.indexOfProperty(property);
    if (propertyIndex < 0)
        return QModelIndex();
    return createIndex(propertyIndex, column, carrierRow);
}

QModelIndex QtIviPropertyModel::indexOfProperty(const QObject *c, const QByteArray &property, int column) const
{
    const int carrierRow = rowOfCarrier(c);
    if (carrierRow < 0)
        return QModelIndex();
    const IviCarrier &carrier = m_carriers.at(carrierRow);
    const int propertyIndex = carrier.indexOfProperty(property);
    if (propertyIndex < 0)
        return QModelIndex();
    return createIndex(propertyIndex, column, carrierRow);
}

void QtIviPropertyModel::objectAdded(QObject *obj)
{
    IF_DEBUG(std::cout << "QtIviPropertyModel::objectAdded() " << obj << std::endl);
    // see Probe::objectCreated, that promises a valid object in the main thread here
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(Probe::instance()->isValidObject(obj));
    int propertyOffset = -1;

    if (qobject_cast<QIviServiceObject*>(obj)) {
        propertyOffset = QIviServiceObject::staticMetaObject.propertyOffset();
    } else if (qobject_cast<QIviAbstractFeature*>(obj)) {
        propertyOffset = QIviAbstractFeature::staticMetaObject.propertyOffset();
    }

    if (propertyOffset != -1) {
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectAdded() - object is an Ivi Object: "
                           << obj << std::endl);
        Q_ASSERT(!m_seenCarriers.contains(obj));
        m_seenCarriers << obj;

        beginInsertRows(QModelIndex(), m_carriers.size(), m_carriers.size());
        m_carriers.push_back(IviCarrier(obj));
        endInsertRows();

        const QMetaObject *mo(obj->metaObject());
        const int propertyCount(mo->propertyCount() - propertyOffset);
        const int featureRow(m_carriers.size() - 1);
        auto &feature(m_carriers.back());

        if (propertyCount > 0) {
            const QModelIndex featureIndex(createIndex(featureRow, 0, -1));
            const int row(feature.propertyCount());

            beginInsertRows(featureIndex, row, row + propertyCount - 1);
            for (int i = 0; i < propertyCount; ++i) {
                const QMetaProperty metaProperty(mo->property(i + propertyOffset));
                QIviProperty *property(metaProperty.read(obj).value<QIviProperty *>());

                if (property) {
                    feature.pushProperty(QtIviPropertyModel::IviCarrierProperty(property, metaProperty));
                    connect(property, &QIviProperty::valueChanged, this, &QtIviPropertyModel::propertyChanged);
                    connect(property, &QIviProperty::availableChanged, this, &QtIviPropertyModel::propertyChanged);
                } else {
                    feature.pushProperty(QtIviPropertyModel::IviCarrierProperty(metaProperty));
                    if (metaProperty.hasNotifySignal()) {
                        const QByteArray normalizedSlot(QMetaObject::normalizedSignature("propertyChanged()"));
                        const int propertyChangedSlotIndex(staticMetaObject.indexOfSlot(normalizedSlot));
                        QMetaObject::connect(obj, metaProperty.notifySignalIndex(), this, propertyChangedSlotIndex);
                    }
                }
            }
            endInsertRows();
        }
    }
}

void QtIviPropertyModel::objectRemoved(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());

    if (m_seenCarriers.contains(obj)) {
        const int row(rowOfCarrier(obj));
        if (row == -1) {
            IF_DEBUG(std::cout << "QtIviPropertyModel::objectRemoved(): we don't know this Ivi Object. "
                     << obj << std::endl);
            return;
        }

        IF_DEBUG(std::cout << "QtIviPropertyModel::objectRemoved(): removing an Ivi Object. "
                 << obj << std::endl);

        beginRemoveRows(QModelIndex(), row, row);
        m_carriers.erase(m_carriers.begin() + row);
        endRemoveRows();
    }
}

void QtIviPropertyModel::objectReparented(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());

    if (m_seenCarriers.contains(obj)) {
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectReparented() " << obj << std::endl);
        // We don't need reparenting:
        // * Plain Qt properties can't move
        // * QIviProperty's are stuck with their parent
    }
}

void QtIviPropertyModel::objectSelected(QObject *obj)
{
    const ObjectId id(obj);
    const QModelIndex index = match(this->index(0, 0), ObjectModel::ObjectIdRole, QVariant::fromValue(id), 1,
                                    Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap).value(0);
    if (!index.isValid()) {
        return;
    }

    QItemSelectionModel *const selectionModel = ObjectBroker::selectionModel(this);
    selectionModel->select(index, QItemSelectionModel::ClearAndSelect |
                           QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

QVariant QtIviPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case NameColumn:
            return tr("Name");
        case ValueColumn:
            return tr("Value");
        case WritableColumn:
            return tr("Writable");
        case OverrideColumn:
            return tr("Override");
        case TypeColumn:
            return tr("Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags QtIviPropertyModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.internalId() != PropertyCarrierIndex) {
        const auto &carrier(m_carriers.at(index.parent().row()));
        const auto &property(carrier.propertyAt(index.row()));

        // We have two levels, this is the second
        flags |= Qt::ItemNeverHasChildren;

        switch (index.column()) {
        case ValueColumn: {
            if (property.isWritable() || property.isOverridable())
                flags |= Qt::ItemIsEditable;
            break;
        }

        case WritableColumn: {
            flags |= Qt::ItemIsUserCheckable;
            if (property.isOverrided() && !property.isWritable())
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEnabled;
            break;
        }

        case OverrideColumn: {
            flags |= Qt::ItemIsUserCheckable;
            if (property.isOverridable() || property.isOverrided())
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEnabled;
            break;
        }

        default:
            break;
        }

        if (!property.isAvailable()) {
            flags &= ~Qt::ItemIsEnabled;
            flags &= ~Qt::ItemIsEditable;
        }
    }

    return flags;
}

QVariant QtIviPropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const quint64 parentRow = index.internalId();
    if (parentRow == PropertyCarrierIndex) {
        // Carrier

        if (index.row() >= 0 && uint(index.row()) < m_carriers.size()) {
            const IviCarrier &carrier = m_carriers.at(index.row());

            switch (role) {
            case Qt::DisplayRole: {
                // The columns are a bit awkward here. They are assigned that way for compatibility
                // with the header data meant for the properties.
                switch (index.column()) {
                case NameColumn:
                    return carrier.label();
                case TypeColumn:
                    return carrier.typeName();
                default:
                    break;
                }

                break;
            }

            case ObjectModel::ObjectIdRole:
                return QVariant::fromValue(carrier.objectId());

            default:
                break;
            }
        }
    } else {
        // Property

        if (parentRow < m_carriers.size()) {
            const IviCarrier &carrier = m_carriers.at(parentRow);

            if (index.row() >= 0 && index.row() < carrier.propertyCount()) {
                const IviCarrierProperty &property = carrier.propertyAt(index.row());

                switch (role) {
                case Qt::DisplayRole: {
                    switch (index.column()) {
                    case NameColumn:
                        return property.name();
                    case ValueColumn:
                        return property.displayText(carrier.m_carrier);
                    case TypeColumn:
                        return property.typeName();
                    default:
                        break;
                    }

                    break;
                }

                case Qt::EditRole: {
                    switch (index.column()) {
                    case ValueColumn:
                        return property.editValue(carrier.m_carrier);
                    default:
                        break;
                    }

                    break;
                }

                case Qt::CheckStateRole: {
                    switch (index.column()) {
                    case WritableColumn:
                        return property.isWritable() || property.isOverrided() ? Qt::Checked : Qt::Unchecked;
                    case OverrideColumn:
                        return property.isOverrided() ? Qt::Checked : Qt::Unchecked;
                    default:
                        break;
                    }

                    break;
                }

                case IsIviProperty: {
                    return property.isOverridable();
                }

                case ValueConstraintsRole: {
                    switch (index.column()) {
                    case ValueColumn:
                        return property.iviConstraints();
                    default:
                        break;
                    }

                    break;
                }

                case ObjectModel::ObjectIdRole:
                    return QVariant::fromValue(property.objectId());

                case RawValue:
                    return property.cppValue(carrier.m_carrier);
                }

            }
        }
    }
    return QVariant();
}

QMap<int, QVariant> QtIviPropertyModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> ret = QAbstractItemModel::itemData(index);
    QVariant maybeConstraints = data(index, ValueConstraintsRole);
    if (maybeConstraints.isValid()) {
        ret.insert(ValueConstraintsRole, maybeConstraints);
    }
    ret.insert(IsIviProperty, data(index, IsIviProperty));
    ret.insert(ObjectModel::ObjectIdRole, data(index, ObjectModel::ObjectIdRole));
    return ret;
}

bool QtIviPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const quint64 parentRow = index.internalId();
    if (!index.isValid() || parentRow == PropertyCarrierIndex ||
            parentRow >= m_carriers.size() || !flags(index).testFlag(Qt::ItemIsEditable)) {
        return false;
    }

    IviCarrier &carrier = m_carriers[parentRow];

    if (index.row() >= 0 && index.row() < carrier.propertyCount()) {
        IviCarrierProperty &property = carrier.propertyAt(index.row());

        switch (index.column()) {
        case ValueColumn: {
            if (role == Qt::DisplayRole || role == Qt::EditRole) {
                if (property.setValue(value, carrier.m_carrier)) {
                    if (!property.hasNotifySignal()) {
                        emitRowDataChanged(index);
                    }

                    return true;
                }
            }

            break;
        }

        case WritableColumn: {
            if (role == Qt::CheckStateRole) {
                // This should only ever be checkable because of a not-really-writable property.
                // Reset the not really writable flag.
                if (!property.isWritable() && property.isOverrided()) {
                    property.setOverrided(false);
                    emitRowDataChanged(index);
                    return true;
                }
            }

            break;
        }

        case OverrideColumn: {
            if (role == Qt::CheckStateRole) {
                const bool isOverride = value.value<Qt::CheckState>() == Qt::Checked;

                if (property.isOverridable() && isOverride != property.isOverrided()) {
                    property.setOverrided(isOverride);
                    emitRowDataChanged(index);
                    return true;
                }
            }

            break;
        }

        default:
            break;
        }
    }

    return false;
}

void QtIviPropertyModel::propertyChanged()
{
    if (QIviProperty *property = qobject_cast<QIviProperty *>(sender())) {
        // An Ivi property changed
        emitRowDataChanged(indexOfProperty(property, NameColumn));
    } else if (qobject_cast<QIviServiceObject *>(sender()) ||
               qobject_cast<QIviAbstractFeature *>(sender())) {
        // A plain Qt property changed in a service or feature
        // Let's update the complete children as we can not known the property that changed
        const QModelIndex parent(indexOfCarrier(sender()));
        Q_ASSERT(parent.isValid());
        const int ccount(columnCount(parent));
        const int rcount(rowCount(parent));
        if (ccount > 0 && rcount > 0) {
            emit dataChanged(index(0, 0, parent), index(rcount - 1, ccount - 1, parent));
        }
    }
}

void QtIviPropertyModel::emitRowDataChanged(const QModelIndex &index)
{
    emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), columnCount() - 1));
}

int QtIviPropertyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() >= 1) {
        return 0;
    }

    if (parent.isValid()) {
        // only carriers have another level of children
        if (parent.internalId() == PropertyCarrierIndex) {
            const IviCarrier &carrier = m_carriers.at(parent.row());
            return carrier.m_properties.size();
        }
        return 0;
    } else {
        return m_carriers.size();
    }
}

int QtIviPropertyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QModelIndex QtIviPropertyModel::parent(const QModelIndex &child) const
{
    if (child.isValid()) {
        const  quint64 parentRow = child.internalId();
        if (parentRow != PropertyCarrierIndex) {
            return createIndex(parentRow, 0, PropertyCarrierIndex);
        }
    }
    return QModelIndex();
}

/*
 Usage of QModelIndex::internalId() aka internalPointer():
 - toplevel (carrier): PropertyCarrierIndex
 - second level (property): index of carrier property (parent)
 */
QModelIndex QtIviPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column >= 0 && column < ColumnCount) {
        if (parent.isValid()) {
            // create an index to a property
            const quint64 grandparentRow = parent.internalId();
            // only carriers have another level of children
            if (grandparentRow == PropertyCarrierIndex &&
                parent.row() >= 0 && uint(parent.row()) < m_carriers.size()) {
                const IviCarrier &carrier = m_carriers.at(parent.row());
                if (row >= 0 && uint(row) < carrier.m_properties.size()) {
                    return createIndex(row, column, parent.row());
                }
            }
        } else {
            // create an index to a carrier
            if (row >= 0 && uint(row) < m_carriers.size()) {
                return createIndex(row, column, PropertyCarrierIndex);
            }
        }
    }
    return QModelIndex();
}
