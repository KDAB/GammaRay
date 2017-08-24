/*
  qtiviobjectmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "qtiviobjectmodel.h"

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
#include <QSignalMapper>

#include <QJsonDocument>
#include <QJsonArray>

#include <iostream>

static const quint64 PropertyCarrierIndex = ~quint64(0); // there is no QUINT64_MAX

//#define IF_DEBUG(x) (x)
#define IF_DEBUG(x)

using namespace GammaRay;

QtIviObjectModel::IviOverriderProperty::IviOverriderProperty()
    : m_overridable(false)
    , m_overriding(false)
{
}

QtIviObjectModel::IviOverriderProperty::IviOverriderProperty(QtIviObjectModel::IviOverriderProperty &&other)
{
    *this = std::move(other);
}

QtIviObjectModel::IviOverriderProperty::IviOverriderProperty(const QMetaProperty &metaProperty, const QVariant &value, bool overridable)
    : m_metaProperty(metaProperty)
    , m_overridable(overridable)
    , m_overriding(false)
    , m_originalValue(value)
{
}

QtIviObjectModel::IviOverriderProperty &QtIviObjectModel::IviOverriderProperty::operator=(QtIviObjectModel::IviOverriderProperty &&other)
{
    m_metaProperty = other.m_metaProperty;
    m_originalValue = other.m_originalValue;
    m_overridenValue = other.m_overridenValue;
    m_overriding = other.m_overriding;
    m_overridable = other.m_overridable;
    return *this;
}

int QtIviObjectModel::IviOverriderProperty::propertyIndex() const
{
    return m_metaProperty.isValid() ? m_metaProperty.propertyIndex() : -1;
}

bool QtIviObjectModel::IviOverriderProperty::isValid() const
{
    return m_metaProperty.isValid();
}

bool QtIviObjectModel::IviOverriderProperty::isAvailable() const
{
    return m_metaProperty.isValid();
}

bool QtIviObjectModel::IviOverriderProperty::hasNotifySignal() const
{
    return m_metaProperty.hasNotifySignal();
}

bool QtIviObjectModel::IviOverriderProperty::isWritable() const
{
    return (m_metaProperty.isWritable() && !QMetaType(m_metaProperty.userType()).flags().testFlag(QMetaType::PointerToQObject));
}

bool QtIviObjectModel::IviOverriderProperty::isOverridable() const
{
    if (!m_overridable)
        return false;
    int propertyOffset = QIviAbstractFeature::staticMetaObject.propertyCount();
    return m_metaProperty.propertyIndex() >= propertyOffset;
}

bool QtIviObjectModel::IviOverriderProperty::isOverriden() const
{
    return m_overriding;
}

QString QtIviObjectModel::IviOverriderProperty::name() const
{
    return QString::fromUtf8(m_metaProperty.name());
}

QString QtIviObjectModel::IviOverriderProperty::typeName() const
{
    const int userType(m_metaProperty.userType());
    return QString::fromLatin1(QMetaType::typeName(userType));
}

ObjectId QtIviObjectModel::IviOverriderProperty::objectId() const
{
    // TODO, something more to add?
    return ObjectId();
}

QString QtIviObjectModel::IviOverriderProperty::displayText() const
{
    const QVariant &value = m_overriding ? m_overridenValue : m_originalValue;
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QString enumStr(EnumUtil::enumToString(value, nullptr, mo));
    if (!enumStr.isEmpty())
        return enumStr;
    return VariantHandler::displayString(value);
}

QVariant QtIviObjectModel::IviOverriderProperty::editValue() const
{
    const QVariant &value = m_overriding ? m_overridenValue : m_originalValue;
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QMetaEnum me(EnumUtil::metaEnum(value, nullptr, mo));
    if (me.isValid()) {
        const int num(EnumUtil::enumToInt(value, me));
        return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
    }
    return VariantHandler::serializableVariant(value);
}

QVariant QtIviObjectModel::IviOverriderProperty::cppValue() const
{
    return m_overriding ? m_overridenValue : m_originalValue;
}

QVariant QtIviObjectModel::IviOverriderProperty::iviConstraints(QIviAbstractFeature* carrier) const
{
    QByteArray constraintsJSON;
    for (int i=0; i<carrier->metaObject()->classInfoCount(); i++) {
        QMetaClassInfo ci = carrier->metaObject()->classInfo(i);
        if (QLatin1String(ci.name()) == QLatin1String("IviPropertyDomains")) {
            constraintsJSON = QByteArray(ci.value());
            break;
        }
    }
    if (constraintsJSON.isEmpty())
        return {};

    QJsonDocument loadDoc = QJsonDocument::fromJson(constraintsJSON);
    if (loadDoc.isNull() || loadDoc.isEmpty() || !loadDoc.isObject())
        return {};

    if (!loadDoc.object().contains(QLatin1String(m_metaProperty.name())))
        return {};

    QJsonValue constraints = loadDoc.object().value(QLatin1String(m_metaProperty.name()));
    if (constraints.toObject().contains("range")) {
        QJsonArray vals = constraints.toObject().value("range").toArray();
        return QVariantList() << quint32(QtIviObjectModel::RangeConstraints) << vals.at(0).toDouble() << vals.at(1).toDouble();
    }
    if (constraints.toObject().contains("minimum")) {
        double val = constraints.toObject().value("minimum").toDouble();
        return QVariantList() << quint32(QtIviObjectModel::MinRangeConstraints) << val;
    }
    if (constraints.toObject().contains("maximum")) {
        double val = constraints.toObject().value("maximum").toDouble();
        return QVariantList() << quint32(QtIviObjectModel::MaxRangeConstraints) << val;
    }
    if (constraints.toObject().contains("domain")) {
        const QVariantList vals = constraints.toObject().value("domain").toArray().toVariantList();
        QVariantList res;
        res << quint32(QtIviObjectModel::AvailableValuesConstraints) << vals;
        return res;
    }
    return {};
}

void QtIviObjectModel::IviOverriderProperty::setOverriden(bool override)
{
    if (override != m_overriding) {
        m_overriding = override;
        if (m_overriding)
            m_overridenValue = m_originalValue;
    }
}

bool QtIviObjectModel::IviOverriderProperty::setOverridenValue(const QVariant &value, QIviAbstractFeature *carrier)
{
    Q_ASSERT(isAvailable());

    QVariant toSet = value;
    if (value.userType() == qMetaTypeId<EnumValue>()) {
        QVariant typeReference = cppValue();
        if (typeReference.type() == QVariant::Int) {
            toSet = value.value<EnumValue>().value();
        } else {
            *(static_cast<int*>(typeReference.data())) = value.value<EnumValue>().value();
            toSet = typeReference;
        }
    }

    const bool wasOverride = isOverriden();
    bool isOverride = wasOverride || !isWritable();

    if (isOverride && !wasOverride && isOverridable())
        setOverriden(isOverride);
    if (isOverride)
        m_overridenValue = toSet;

    if (m_metaProperty.isWritable())
        return m_metaProperty.write(carrier, toSet);
    else
        return notifyOverridenValue(value, carrier);
}

bool QtIviObjectModel::IviOverriderProperty::notifyOverridenValue(const QVariant &value, QIviAbstractFeature *carrier)
{
    QMetaMethod notifySignal = m_metaProperty.notifySignal();
    if (!notifySignal.isValid() || notifySignal.parameterCount() != 1)
        return false;

    switch (value.type()) {
    case QVariant::Int: return notifySignal.invoke(carrier, Q_ARG(int, value.value<int>()));
    case QVariant::String: return notifySignal.invoke(carrier, Q_ARG(QString, value.value<QString>()));
    case QVariant::Double: return notifySignal.invoke(carrier, Q_ARG(double, value.value<double>()));
    case QVariant::Bool: return notifySignal.invoke(carrier, Q_ARG(double, value.value<bool>()));
    default:
        return false;
    }
}

void QtIviObjectModel::IviOverriderProperty::setOriginalValue(const QVariant &editValue)
{
    m_originalValue = editValue;
}

bool QtIviObjectModel::IviOverriderProperty::operator==(const QByteArray &property) const
{
    return this->m_metaProperty.name() == property;
}


QtIviObjectModel::IviOverrider::IviOverrider()
    : m_serviceObject(nullptr)
{
}

QtIviObjectModel::IviOverrider::IviOverrider(QIviAbstractFeature *carrier)
    : m_serviceObject(nullptr)
{
    if(carrier) {
        m_serviceObject = carrier->serviceObject();
        m_carriers.push_back(carrier);
    }
}

QtIviObjectModel::IviOverrider::~IviOverrider()
{
    for(auto &c : qAsConst(m_carriers))
        setCarrierOverride(false, c);
}

void QtIviObjectModel::IviOverrider::addCarrier(QIviAbstractFeature *carrier)
{
    m_carriers.push_back(carrier);
    setCarrierOverride(true, carrier);
}

void QtIviObjectModel::IviOverrider::removeCarrier(QIviAbstractFeature *carrier)
{
    setCarrierOverride(false, carrier);
    m_carriers.erase(std::remove_if(m_carriers.begin(), m_carriers.end(), [carrier](QIviAbstractFeature *c) {
        return carrier == c;
    }), m_carriers.end());
}

void QtIviObjectModel::IviOverrider::setCarrierOverride(bool override, QIviAbstractFeature *carrier)
{
    if (!carrier)
        return;
    QIviAbstractFeaturePrivate *const pPriv = QIviAbstractFeaturePrivate::get(carrier);
    if (!pPriv)
        return;
    if (override && pPriv->m_propertyOverride == nullptr) {
        pPriv->m_propertyOverride = this;
    } else if (!override && pPriv->m_propertyOverride == this) {
        pPriv->m_propertyOverride = nullptr;
    }
}

int QtIviObjectModel::IviOverrider::numCarriers() const
{
    return (int) m_carriers.size();
}

QVariant QtIviObjectModel::IviOverrider::property(int propertyIndex) const
{
    if (m_carriers.empty())
        return {};

    const IviOverriderProperty &property = propertyForIndex(propertyIndex);
    return property.cppValue();
}

void QtIviObjectModel::IviOverrider::setProperty(int propertyIndex, const QVariant &value)
{
    if (m_carriers.empty())
        return;

    IviOverriderProperty &property = propertyForIndex(propertyIndex);
    property.setOriginalValue(value);
}

bool QtIviObjectModel::IviOverrider::isOverridden(int propertyIndex) const
{
    if (m_carriers.empty())
        return false;

    const IviOverriderProperty &property = propertyForIndex(propertyIndex);
    return property.isOverriden();
}

QVariant QtIviObjectModel::IviOverrider::iviConstraints(int propertyIndex) const
{
    if (m_carriers.empty())
        return {};

    const IviOverriderProperty &property = propertyForIndex(propertyIndex);
    return property.iviConstraints(m_carriers.front());
}

QString QtIviObjectModel::IviOverrider::label() const
{
    if (m_carriers.empty())
        return {};
    QString name = m_serviceObject ? m_serviceObject->objectName() : QString();
    if (name.isEmpty())
        name = typeName();
    if (const QIviAbstractZonedFeature *zoned = qobject_cast<const QIviAbstractZonedFeature *>(m_carriers.front()))
        // not translated; the zone API is fixed to English, too
        name += QString::fromLatin1(" [Zone: %1]").arg(zoned->zone());
    return name;
}

QString QtIviObjectModel::IviOverrider::description() const
{
    if (!m_serviceObject)
        return {};
    return QString("Backend Type: %1\nInstances in zone: %2")
            .arg(QString::fromLatin1(m_serviceObject->metaObject()->className()))
            .arg(m_carriers.size());
}

QString QtIviObjectModel::IviOverrider::typeName() const
{
    if (m_carriers.empty())
        return {};
    return QString::fromLatin1(m_carriers.front()->metaObject()->className());
}

ObjectId QtIviObjectModel::IviOverrider::objectId() const
{
    return ObjectId(m_serviceObject);
}

bool QtIviObjectModel::IviOverrider::setOverride(int index, bool isOverride)
{
    IviOverriderProperty &property = propertyAt(index);
    if (property.isOverridable() && isOverride != property.isOverriden()) {
        if (!isOverride) {
            QByteArray flag = QString("%1DirtyOverride").arg(property.name()).toLatin1();
            for (const auto &carrier : m_carriers) {
                carrier->setProperty(flag.data(), true);
                property.setOverridenValue(property.m_originalValue, carrier);
            }
        }
        property.setOverriden(isOverride);
        return true;
    }
    return false;
}

bool QtIviObjectModel::IviOverrider::setOverridenValue(int index, const QVariant &value)
{
    IviOverriderProperty &property = propertyAt(index);
    bool res = false;
    for (const auto &carrier : m_carriers) {
        if(property.setOverridenValue(value, carrier))
            res = true;
    }
    return true;
}

int QtIviObjectModel::IviOverrider::propertyCount() const
{
    return m_properties.size();
}

void QtIviObjectModel::IviOverrider::pushProperty(IviOverriderProperty &&property)
{
    m_properties.push_back(std::move(property));
}

const QtIviObjectModel::IviOverriderProperty &QtIviObjectModel::IviOverrider::propertyAt(int index) const
{
    return m_properties.at(index);
}

const QtIviObjectModel::IviOverriderProperty &QtIviObjectModel::IviOverrider::propertyForIndex(int index) const
{
    static QtIviObjectModel::IviOverriderProperty dummy;
    for (const auto &p: m_properties) {
        if (p.propertyIndex() == index)
            return p;
    }
    return dummy;
}

QtIviObjectModel::IviOverriderProperty &QtIviObjectModel::IviOverrider::propertyForIndex(int index)
{
    static QtIviObjectModel::IviOverriderProperty dummy;
    for (auto &p: m_properties) {
        if (p.propertyIndex() == index)
            return p;
    }
    return dummy;
}

QtIviObjectModel::IviOverriderProperty &QtIviObjectModel::IviOverrider::propertyAt(int index)
{
    return m_properties.at(index);
}

int QtIviObjectModel::IviOverrider::indexOfProperty(const QByteArray &property) const
{
    for (uint i = 0; i < m_properties.size(); i++) {
        if (m_properties.at(i) == property) {
            return i;
        }
    }
    return -1;
}

bool QtIviObjectModel::IviOverrider::handles(const QObject *carrier) const
{
    return std::find_if(m_carriers.begin(), m_carriers.end(), [carrier](const QIviAbstractFeature *p) {
        return p == carrier;
    }) != m_carriers.end();
}

QString QtIviObjectModel::IviOverrider::serviceId() const
{
    return m_serviceObject ? m_serviceObject->id() : QString();
}



QtIviObjectModel::QtIviObjectModel(Probe *probe)
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

int QtIviObjectModel::rowOfCarrier(const QObject *carrier) const
{
    for (uint i = 0; i < m_serviceCarriers.size(); i++) {
        if (m_serviceCarriers.at(i).m_service->handles(carrier)) {
            return i;
        }
    }
    return -1;
}

QModelIndex QtIviObjectModel::indexOfCarrier(const QObject *carrier, int column) const
{
    const int row(rowOfCarrier(carrier));
    return row == -1 ? QModelIndex() : index(row, column);
}

QModelIndex QtIviObjectModel::indexOfProperty(const QObject *c, const QByteArray &property, int column) const
{
    const int carrierRow = rowOfCarrier(c);
    if (carrierRow < 0)
        return QModelIndex();
    const auto &carrier = m_serviceCarriers.at(carrierRow).m_service;
    const int propertyIndex = carrier->indexOfProperty(property);
    if (propertyIndex < 0)
        return QModelIndex();
    return createIndex(propertyIndex, column, carrierRow);
}

void QtIviObjectModel::objectAdded(QObject *obj)
{
    IF_DEBUG(std::cout << "QtIviObjectModel::objectAdded() " << obj << std::endl);
    // see Probe::objectCreated, that promises a valid object in the main thread here
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(Probe::instance()->isValidObject(obj));
    int propertyOffset = QIviAbstractFeature::staticMetaObject.propertyCount();

    QIviAbstractFeature *featureObj = qobject_cast<QIviAbstractFeature *>(obj);
    QString serviceId = featureObj && featureObj->serviceObject() ? featureObj->serviceObject()->id() : QString();
    QString zoneId;
    if (const QIviAbstractZonedFeature *zoned = qobject_cast<const QIviAbstractZonedFeature *>(featureObj)) {
        zoneId = zoned->zone();
    }

    if (!serviceId.isEmpty()) {
        IF_DEBUG(std::cout << "QtIviObjectModel::objectAdded() - object is an Ivi Object: "
                           << obj << std::endl);
        Q_ASSERT(!m_handledObjects.contains(obj));
        m_handledObjects << obj;

        auto it = std::find_if(m_serviceCarriers.begin(), m_serviceCarriers.end(), [serviceId, zoneId](const ServiceZone &p) {
            return p.m_service->serviceId() == serviceId && p.m_zone == zoneId;
        });
        if (it != m_serviceCarriers.end()) {
            (*it).m_service->addCarrier(featureObj);
        } else {
            beginInsertRows(QModelIndex(), m_serviceCarriers.size(), m_serviceCarriers.size());
            ServiceZone sz;
            sz.m_zone = zoneId;
            sz.m_service = std::make_shared<IviOverrider>(featureObj);
            m_serviceCarriers.push_back(sz);
            endInsertRows();

            const QMetaObject *mo(obj->metaObject());
            const int propertyCount(mo->propertyCount() - propertyOffset);
            const int featureRow(m_serviceCarriers.size() - 1);
            IviOverriderPtr &feature(sz.m_service);

            if (propertyCount > 0) {
                const QModelIndex featureIndex(createIndex(featureRow, 0, -1));
                const int row(feature->propertyCount());

                QIviAbstractFeaturePrivate *featureObjPrivate = QIviAbstractFeaturePrivate::get(featureObj);
                const bool canOveride = featureObjPrivate && featureObjPrivate->m_supportsPropertyOverriding;

                beginInsertRows(featureIndex, row, row + propertyCount - 1);
                for (int i = 0; i < propertyCount; ++i) {
                    const QMetaProperty metaProperty(mo->property(i + propertyOffset));
                    feature->pushProperty(QtIviObjectModel::IviOverriderProperty(metaProperty, metaProperty.read(obj), canOveride));
                    if (metaProperty.hasNotifySignal()) {
                        const QByteArray normalizedSlot(QMetaObject::normalizedSignature("propertyChanged()"));
                        const int propertyChangedSlotIndex(staticMetaObject.indexOfSlot(normalizedSlot));
                        QMetaObject::connect(obj, metaProperty.notifySignalIndex(), this, propertyChangedSlotIndex);
                    }
                }
                endInsertRows();
                feature->setCarrierOverride(true, featureObj);
            }
        }
    }
}

void QtIviObjectModel::objectRemoved(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());

    if (m_handledObjects.contains(obj)) {
        QIviAbstractFeature *featureObj = qobject_cast<QIviAbstractFeature *>(obj);
        for(auto &c : m_serviceCarriers) {
            if(c.m_service->handles(obj)) {
                c.m_service->removeCarrier(featureObj);

                if (c.m_service->numCarriers() == 0) {
                    const int row(rowOfCarrier(obj));
                    beginRemoveRows(QModelIndex(), row, row);
                    m_serviceCarriers.erase(m_serviceCarriers.begin() + row);
                    endRemoveRows();
                }
                break;
            }
        }
    }
}

void QtIviObjectModel::objectReparented(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());

    if (m_handledObjects.contains(obj)) {
        IF_DEBUG(std::cout << "QtIviObjectModel::objectReparented() " << obj << std::endl);
        // We don't need reparenting:
        // * Plain Qt properties can't move
        // * QIviProperty's are stuck with their parent
    }
}

void QtIviObjectModel::objectSelected(QObject *obj)
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

QVariant QtIviObjectModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags QtIviObjectModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.internalId() != PropertyCarrierIndex) {
        const auto &carrier(m_serviceCarriers.at(index.parent().row()).m_service);
        const auto &property(carrier->propertyAt(index.row()));

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
            if (property.isOverriden() && !property.isWritable())
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEnabled;
            break;
        }

        case OverrideColumn: {
            flags |= Qt::ItemIsUserCheckable;
            if (property.isOverridable() || property.isOverriden())
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

QVariant QtIviObjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const quint64 parentRow = index.internalId();
    if (parentRow == PropertyCarrierIndex) {
        // Carrier

        if (index.row() >= 0 && uint(index.row()) < m_serviceCarriers.size()) {
            const auto &carrier = m_serviceCarriers.at(index.row()).m_service;

            switch (role) {
            case Qt::DisplayRole: {
                // The columns are a bit awkward here. They are assigned that way for compatibility
                // with the header data meant for the properties.
                switch (index.column()) {
                case NameColumn:
                    return carrier->label();
                case TypeColumn:
                    return carrier->typeName();
                default:
                    break;
                }

                break;
            }

            case Qt::ToolTipRole:
                return carrier->description();

            case ObjectModel::ObjectIdRole:
                return QVariant::fromValue(carrier->objectId());

            default:
                break;
            }
        }
    } else {
        // Property

        if (parentRow < m_serviceCarriers.size()) {
            const auto &carrier = m_serviceCarriers.at(parentRow).m_service;

            if (index.row() >= 0 && index.row() < carrier->propertyCount()) {
                const IviOverriderProperty &property = carrier->propertyAt(index.row());

                switch (role) {
                case Qt::DisplayRole: {
                    switch (index.column()) {
                    case NameColumn:
                        return property.name();
                    case ValueColumn:
                        return property.displayText();
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
                        return property.editValue();
                    default:
                        break;
                    }

                    break;
                }

                case Qt::CheckStateRole: {
                    switch (index.column()) {
                    case WritableColumn:
                        return property.isWritable() || property.isOverriden() ? Qt::Checked : Qt::Unchecked;
                    case OverrideColumn:
                        return property.isOverriden() ? Qt::Checked : Qt::Unchecked;
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
                        return carrier->iviConstraints(index.row());
                    default:
                        break;
                    }

                    break;
                }

                case ObjectModel::ObjectIdRole:
                    return QVariant::fromValue(property.objectId());

                case RawValue:
                    return property.cppValue();
                }

            }
        }
    }
    return QVariant();
}

QMap<int, QVariant> QtIviObjectModel::itemData(const QModelIndex &index) const
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

bool QtIviObjectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const quint64 parentRow = index.internalId();
    if (!index.isValid() || parentRow == PropertyCarrierIndex ||
            parentRow >= m_serviceCarriers.size() || !flags(index).testFlag(Qt::ItemIsEditable)) {
        return false;
    }

    auto &carrier = m_serviceCarriers[parentRow].m_service;

    if (index.row() >= 0 && index.row() < carrier->propertyCount()) {
        IviOverriderProperty &property = carrier->propertyAt(index.row());

        switch (index.column()) {
        case ValueColumn: {
            if (role == Qt::DisplayRole || role == Qt::EditRole) {
                if (carrier->setOverridenValue(index.row(), value)) {
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
                if (!property.isWritable() && property.isOverriden()) {
                    property.setOverriden(false);
                    emitRowDataChanged(index);
                    return true;
                }
            }

            break;
        }

        case OverrideColumn: {
            if (role == Qt::CheckStateRole) {
                const bool isOverride = value.value<Qt::CheckState>() == Qt::Checked;

                if(carrier->setOverride(index.row(), isOverride))
                    return true;
            }

            break;
        }

        default:
            break;
        }
    }

    return false;
}

void QtIviObjectModel::propertyChanged()
{
    if (qobject_cast<QIviServiceObject *>(sender()) || qobject_cast<QIviAbstractFeature *>(sender())) {
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

void QtIviObjectModel::emitRowDataChanged(const QModelIndex &index)
{
    emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), columnCount() - 1));
}

int QtIviObjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() >= 1) {
        return 0;
    }

    if (parent.isValid()) {
        // only carriers have another level of children
        if (parent.internalId() == PropertyCarrierIndex) {
            const auto &carrier = m_serviceCarriers.at(parent.row()).m_service;
            return carrier->m_properties.size();
        }
        return 0;
    } else {
        return m_serviceCarriers.size();
    }
}

int QtIviObjectModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QModelIndex QtIviObjectModel::parent(const QModelIndex &child) const
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
QModelIndex QtIviObjectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column >= 0 && column < ColumnCount) {
        if (parent.isValid()) {
            // create an index to a property
            const quint64 grandparentRow = parent.internalId();
            // only carriers have another level of children
            if (grandparentRow == PropertyCarrierIndex &&
                parent.row() >= 0 && uint(parent.row()) < m_serviceCarriers.size()) {
                const auto &carrier = m_serviceCarriers.at(parent.row()).m_service;
                if (row >= 0 && uint(row) < carrier->m_properties.size()) {
                    return createIndex(row, column, parent.row());
                }
            }
        } else {
            // create an index to a carrier
            if (row >= 0 && uint(row) < m_serviceCarriers.size()) {
                return createIndex(row, column, PropertyCarrierIndex);
            }
        }
    }
    return QModelIndex();
}
