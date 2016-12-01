/*
  qtivipropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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
#include <common/objectmodel.h>

#include <QIviAbstractZonedFeature>
#include <QIviProperty>

#include <QThread>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMutexLocker>
#include <QSignalMapper>

#include <iostream>

static const quint64 PropertyCarrierIndex = ~quint64(0); // there is no QUINT64_MAX

//#define IF_DEBUG(x) (x)
#define IF_DEBUG(x)

using namespace GammaRay;

enum Columns
{
    NameColumn = 0,
    ValueColumn,
    WritableColumn,
    OverrideColumn,
    TypeColumn,
    ColumnCount
};

QtIviPropertyModel::QtIviPropertyModel(Probe *probe)
    : QAbstractItemModel(probe)
{
    connect(probe, SIGNAL(objectCreated(QObject*)), this, SLOT(objectAdded(QObject*)));
    connect(probe, SIGNAL(objectDestroyed(QObject*)), this, SLOT(objectRemoved(QObject*)));
    connect(probe, SIGNAL(objectReparented(QObject*)), this, SLOT(objectReparented(QObject*)));
}

QtIviPropertyModel::IviProperty::IviProperty(QIviProperty *_value, const QMetaProperty &metaProperty)
   : name(QString::fromUtf8(metaProperty.name())),
     value(_value),
     notWritableInPractice(false),
     overrider(_value)
{
}

QtIviPropertyModel::IviProperty::IviProperty()
   : value(nullptr)
{
}

// The out-argument is the easiest way to allow for the case that no matching QMetaProperty was found
static bool addIviProperty(const QObject *carrier, const QIviProperty *property,
                           QtIviPropertyModel::IviProperty *modelProperty)
{
    //int qtIviPropertyType = qMetaTypeId<QIviProperty *>(); // TODO register the metatype in QtIvi
    QByteArray qtIviPropertyTypeName("QIviProperty*");
    const QMetaObject *mo = carrier->metaObject();
    static const int count = mo->propertyCount();
    for (int i = 0; i < count; i++) {
        const QMetaProperty mp = mo->property(i);
        //if (mp.userType() == qtIviPropertyType) {
        if (qtIviPropertyTypeName == mp.typeName()) {
            QIviProperty *value = *reinterpret_cast<QIviProperty **>(mp.read(carrier).data());
            if (value == property) {
                *modelProperty = QtIviPropertyModel::IviProperty(value, mp);
                return true;
            }
        }
    }
    return false;
}

int QtIviPropertyModel::indexOfPropertyCarrier(const QObject *carrier) const
{
    for (uint i = 0; i < m_propertyCarriers.size(); i++) {
        if (m_propertyCarriers.at(i).carrier == carrier) {
            return i;
        }
    }
    return -1;
}

int QtIviPropertyModel::IviPropertyCarrier::indexOfProperty(const QIviProperty *property) const
{
    for (uint i = 0; i < iviProperties.size(); i++) {
        if (iviProperties.at(i).value == property) {
            return i;
        }
    }
    return -1;
}

void QtIviPropertyModel::objectAdded(QObject *obj)
{
    // We see a property carriers in a half-constructed state here, let's take QIviClimateControl as
    // an example. The QMetaObject::className() is "QIviClimateControl", but the list of properties
    // is that of the superclass QIviAbstractFeature. So we need to link property carriers with
    // their properties later than immediately upon on creation of the carrier. We could:
    // - enqueue all created objects internally and batch-process them at the next return to event
    //   loop, using a zero-timeout timer or an event
    // - add the property carrier (parent) on demand when a child property is observed
    // None of them is very good because both rely on behavior that isn't really guaranteed.
    // For now, we're using the second option.

    IF_DEBUG(std::cout << "QtIviPropertyModel::objectAdded() " << obj << std::endl);
    // see Probe::objectCreated, that promises a valid object in the main thread here
    Q_ASSERT(thread() == QThread::currentThread());
    Q_ASSERT(Probe::instance()->isValidObject(obj));
    Q_ASSERT(!m_seenObjects.contains(obj));

    if (QIviProperty *property = qobject_cast<QIviProperty*>(obj)) {
        QObject *carrier = property->parent();
        // Things that turned out to be true in practice, check that they stay true
        Q_ASSERT(carrier);
        // Q_ASSERT(m_seenObjects.contains(carrier)); ...
        // should always be true except for one problem: Our gammaray_qtivi.json file declares that
        // this plugin should only be loaded when instances of certain classes appear for the first
        // time (a Gammaray mechanism to reduce overhead); QObject is not in that list because we
        // only *really* care about a few special QObjects - QIviProperties and their parents.
        // The parents don't trigger loading of the plugin so shortly after being loaded we will
        // see objects with not yet known parents.
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectAdded() - parent is a carrier "
                           << carrier << std::endl);

        const bool wasKnownCarrier = m_seenObjects.value(carrier);
        int carrierRow = -1;
        std::vector<IviProperty> *iviProperties = nullptr;

        if (!wasKnownCarrier) {
            m_seenObjects.insert(carrier, true);

            IviPropertyCarrier carrierStruct;
            carrierStruct.carrier = carrier;

            beginInsertRows(QModelIndex(), m_propertyCarriers.size(), m_propertyCarriers.size());
            m_propertyCarriers.push_back(std::move(carrierStruct));
            endInsertRows();

            carrierRow = m_propertyCarriers.size() - 1;
            iviProperties = &m_propertyCarriers.back().iviProperties;
        } else {
            carrierRow = indexOfPropertyCarrier(carrier);
            iviProperties = &m_propertyCarriers[carrierRow].iviProperties;
            Q_ASSERT(carrierRow != -1);
        }

        QtIviPropertyModel::IviProperty iviProperty;
        if (addIviProperty(carrier, property, &iviProperty)) {
            beginInsertRows(createIndex(carrierRow, 0, -1),
                            iviProperties->size(), iviProperties->size());
            iviProperties->push_back(std::move(iviProperty));
            endInsertRows();
        } else {
            std::cout << "QtIviPropertyModel::objectAdded() - add property FAILED " << property << std::endl;
        }

        connect(property, &QIviProperty::valueChanged, this, &QtIviPropertyModel::propertyValueChanged);
        connect(property, &QIviProperty::availableChanged, this, &QtIviPropertyModel::availabilityChanged);
    }

    m_seenObjects.insert(obj, false);
}

void QtIviPropertyModel::objectRemoved(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());

    QHash<QObject *, bool>::Iterator objIt = m_seenObjects.find(obj);
    if (objIt == m_seenObjects.end()) {
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectRemoved(): we don't know this. "
                           << obj << std::endl);
        return;
    }
    if (!objIt.value()) {
        // HACK: assuming that the parent will be removed before anyone looks at the data and therefore
        // the current object; it might be better to go to the other extreme and remove the parent and
        // all siblings instead.
        // Removing object by object is also possible if we really have to. But currently all QIviProperties
        // seem to be children of the objects that have them as properties, so they are most likely
        // destroyed together. That parent-child connection is also assumed in the rest of this class.

        // An important part of this hack is not tipping off the model's clients about *any* changes
        // while properties are being removed. Only remove their parent and then notify about it.
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectRemoved(): not a QIviProperty carrier. "
                           << obj << std::endl);
        m_seenObjects.erase(objIt);
        return;
    }

    int carrierIndex = -1;
    for (uint i = 0; i < m_propertyCarriers.size(); i++) {
        if (m_propertyCarriers.at(i).carrier == obj) {
            carrierIndex = i;
            break;
        }
    }
    Q_ASSERT(carrierIndex != -1); // inconsistent with m_seenObjects

    IF_DEBUG(std::cout << "QtIviPropertyModel::objectRemoved(): removing a QIviProperty carrier. "
                       << obj << std::endl);

    // ### Do we need to notify about removing the children? It should be obvious that they're
    // removed with the parent...
    beginRemoveRows(QModelIndex(), carrierIndex, carrierIndex);
    m_seenObjects.erase(objIt);
    m_propertyCarriers.erase(m_propertyCarriers.begin() + carrierIndex);
    endRemoveRows();
}

void QtIviPropertyModel::objectReparented(QObject *obj)
{
    // slot, hence should always land in main thread due to auto connection
    Q_ASSERT(thread() == QThread::currentThread());
    if (m_seenObjects.contains(obj)) {
        IF_DEBUG(std::cout << "QtIviPropertyModel::objectReparented() " << obj << std::endl);
        // TODO do we need to handle this? If we may assume that the parent-child
        // connection between property carriers and properties always exists, then probably not.
    }
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
        const auto &carrier = m_propertyCarriers.at(index.parent().row());
        const auto &prop = carrier.iviProperties.at(index.row());
        if (!prop.value->isAvailable())
            flags &= ~Qt::ItemIsEnabled;
        if (index.column() == ValueColumn) {
            flags |= Qt::ItemIsEditable;
        } else if (index.column() == OverrideColumn) {
            flags |= Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
        }
    }
    return flags;
}

static QString buildCarrierObjectName(const QObject *carrier)
{
    QString name = carrier->objectName();
    if (name.isEmpty()) {
        if (const QIviAbstractZonedFeature *zoned = qobject_cast<const QIviAbstractZonedFeature *>(carrier)) {
            name = QLatin1String("Zone: ") + // not translated; the zone API is fixed to English, too
                   zoned->zone();
        }
    }
    if (name.isEmpty()) {
        name = QString::fromLatin1(carrier->metaObject()->className()) +
               QLatin1String(" 0x") + QString::number(quintptr(carrier), 16);
    }
    return name;
}

QVariant QtIviPropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    const quint64 parentRow = index.internalId();
    if (parentRow == PropertyCarrierIndex && role == Qt::DisplayRole) {
        // property carrier
        if (index.row() >= 0 && uint(index.row()) < m_propertyCarriers.size()) {
            const IviPropertyCarrier &propCarrier = m_propertyCarriers.at(index.row());
            // The columns are a bit awkward here. They are assigned that way for compatibility
            // with the header data meant for the properties.
            switch (index.column()) {
            case NameColumn:
                return buildCarrierObjectName(propCarrier.carrier);
            case TypeColumn:
                return QString::fromLatin1(propCarrier.carrier->metaObject()->className());
            default:
                break;
            }
        }
    } else {
        // property

        // Note about value access: QIviProperty sanitizes values for QML, it turns enums and flags
        // into ints. This is not what we want here. Use QtIviPropertyOverrider to bypass the
        // filtering.

        if (parentRow != PropertyCarrierIndex && parentRow < m_propertyCarriers.size()) {
            const IviPropertyCarrier &propCarrier = m_propertyCarriers.at(parentRow);
            if (index.row() >= 0 && uint(index.row()) < propCarrier.iviProperties.size()) {
                const IviProperty &iviProperty = propCarrier.iviProperties.at(index.row());
                switch (index.column()) {
                case NameColumn:
                    if (role == Qt::DisplayRole) {
                        return iviProperty.name;
                    }
                    break;
                case ValueColumn: {
                    if (role == Qt::DisplayRole) {
                        const QVariant value = iviProperty.overrider.value();
                        const QMetaObject *const mo = QMetaType::metaObjectForType(value.userType());
                        const QString enumStr = EnumUtil::enumToString(value, nullptr, mo);
                        if (!enumStr.isEmpty())
                            return enumStr;
                        return VariantHandler::displayString(value);
                    } else if (role == Qt::EditRole) {
                        const QVariant value = iviProperty.overrider.value();
                        const QMetaObject *const mo = QMetaType::metaObjectForType(value.userType());
                        const QMetaEnum me = EnumUtil::metaEnum(value, nullptr, mo);
                        if (me.isValid()) {
                            const int num = EnumUtil::enumToInt(value, me);
                            return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
                        }
                        return VariantHandler::serializableVariant(value);
                    }
                    break;
                }
                case WritableColumn:
                    if (role == Qt::CheckStateRole) {
                        const bool writable = iviProperty.overrider.userWritable() &&
                                              !iviProperty.notWritableInPractice;
                        return writable ? Qt::Checked : Qt::Unchecked;
                    }
                    break;
                case OverrideColumn:
                    if (role == Qt::CheckStateRole) {
                        return iviProperty.overrider.isOverride() ? Qt::Checked : Qt::Unchecked;
                    }
                    break;
                case TypeColumn:
                    if (role == Qt::DisplayRole) {
                        QVariant value = iviProperty.overrider.value();
                        const int metatype = value.userType();
                        return QString::fromLatin1(QMetaType::typeName(metatype));
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    return QVariant();
}

bool QtIviPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int parentRow = static_cast<int>(index.internalId());
    if (!index.isValid() || (index.column() != ValueColumn && index.column() != OverrideColumn)
        || index.internalId() == PropertyCarrierIndex) {
        return false;
    }

    // Note about value access: The comment from data() applies. However, when we imitate the
    // valueChanged() signal, it is better to use the filtered value, so get it from
    // QIviProperty::value() in that case.

    if (parentRow >= 0 && uint(parentRow) < m_propertyCarriers.size()) {
        IviPropertyCarrier *propCarrier = &m_propertyCarriers[parentRow];
        if (index.row() >= 0 && uint(index.row()) < propCarrier->iviProperties.size()) {
            IviProperty *iviProperty = &propCarrier->iviProperties[index.row()];
            if (index.column() == ValueColumn) {
                if (role == Qt::DisplayRole || role == Qt::EditRole) {
                    const bool wasOverride = iviProperty->overrider.isOverride();
                    bool isOverride = wasOverride || !iviProperty->overrider.userWritable() ||
                                      iviProperty->notWritableInPractice;
                    bool writableChanged = false;
                    bool overrideChanged = false;

                    if (isOverride && !wasOverride) {
                        // Don't receive valueChanged signals we caused ourselves. We emit the
                        // dataChanged() signal manually instead, which has less potential for
                        // "interesting" side effects.
                        disconnect(iviProperty->value, &QIviProperty::valueChanged,
                                   this, &QtIviPropertyModel::propertyValueChanged);
                        iviProperty->overrider.setOverride(isOverride);
                        overrideChanged = true;
                    }

                    QVariant toSet = value;
                    if (value.userType() == qMetaTypeId<EnumValue>()) {
                        QVariant typeReference = iviProperty->overrider.value();
                        if (typeReference.type() == QVariant::Int) {
                            toSet = value.value<EnumValue>().value();
                        } else {
                            *(static_cast<int*>(typeReference.data())) = value.value<EnumValue>().value();
                            toSet = typeReference;
                        }
                    }
                    iviProperty->overrider.setValue(toSet);

                    // Hack: some properties reject value changes with no general way to know that
                    // up front, so check and compensate similarly to "proper" read-only properties.
                    if (!isOverride && iviProperty->overrider.value() != toSet) {
                        disconnect(iviProperty->value, &QIviProperty::valueChanged,
                                   this, &QtIviPropertyModel::propertyValueChanged);
                        isOverride = true;
                        overrideChanged = true;
                        iviProperty->notWritableInPractice = true;
                        writableChanged = true;
                        iviProperty->overrider.setOverride(isOverride);
                        iviProperty->overrider.setValue(toSet);
                    }

                    // Note that writableChanged and valueChanged can only be true if the original
                    // setter wasn't called or had no effect, so the signal will even always be
                    // emitted in cause-effect order. Not that it matters in most cases...
                    if (writableChanged) {
                        emit dataChanged(index, index.sibling(index.row(), WritableColumn));
                    }
                    if (overrideChanged) {
                        emit dataChanged(index, index.sibling(index.row(), OverrideColumn));
                    }
                    if (isOverride) {
                        emit iviProperty->value->valueChanged(iviProperty->value->value());
                        emit dataChanged(index, index);
                    } // ... else the valueChanged() signal was hopefully emitted from the setter
                    return true;
                }
            } else if (index.column() == OverrideColumn) {
                if (role == Qt::CheckStateRole) {
                    const bool wasOverride = iviProperty->overrider.isOverride();
                    const bool isOverride = value.toBool();
                    if (isOverride != wasOverride) {
                        emit dataChanged(index.sibling(index.row(), OverrideColumn), index);
                        if (isOverride) {
                            disconnect(iviProperty->value, &QIviProperty::valueChanged,
                                       this, &QtIviPropertyModel::propertyValueChanged);
                            // for the initial override value, keep the "real" value from original backend
                            const QVariant originalValue = iviProperty->overrider.value();
                            iviProperty->overrider.setOverride(true);
                            iviProperty->overrider.setValue(originalValue);
                        } else {
                            // revert to value from original backend
                            iviProperty->overrider.setOverride(false);
                            connect(iviProperty->value, &QIviProperty::valueChanged,
                                    this, &QtIviPropertyModel::propertyValueChanged);
                            emit iviProperty->value->valueChanged(iviProperty->value->value());
                        }
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void QtIviPropertyModel::propertyValueChanged(const QVariant &)
{
    QIviProperty *property = qobject_cast<QIviProperty *>(sender());
    if (!property) {
        return;
    }
    const int carrierIndex = indexOfPropertyCarrier(property->parent());
    if (carrierIndex < 0) {
        return;
    }
    const IviPropertyCarrier &carrier = m_propertyCarriers.at(carrierIndex);
    const int propertyIndex = carrier.indexOfProperty(property);
    if (propertyIndex < 0) {
        return;
    }
    const QModelIndex index = createIndex(propertyIndex, ValueColumn, carrierIndex);
    emit dataChanged(index, index);
}

void QtIviPropertyModel::availabilityChanged()
{
    auto property = qobject_cast<QIviProperty *>(sender());
    if (!property)
        return;
    const auto carrierIndex = indexOfPropertyCarrier(property->parent());
    if (carrierIndex < 0)
        return;
    const auto &carrier = m_propertyCarriers.at(carrierIndex);
    const auto propertyIndex = carrier.indexOfProperty(property);
    if (propertyIndex < 0)
        return;
    emit dataChanged(createIndex(propertyIndex, NameColumn, carrierIndex), createIndex(propertyIndex, TypeColumn, carrierIndex));
}

int QtIviPropertyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() >= 1) {
        return 0;
    }

    if (parent.isValid()) {
        // only property carriers have another level of children
        if (parent.internalId() == PropertyCarrierIndex) {
            const IviPropertyCarrier &propCarrier = m_propertyCarriers.at(parent.row());
            return propCarrier.iviProperties.size();
        }
        return 0;
    } else {
        return m_propertyCarriers.size();
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
 - toplevel (property carrier): PropertyCarrierIndex
 - second level (property): index of property carrier (parent)
 */
QModelIndex QtIviPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column >= 0 && column < ColumnCount) {
        if (parent.isValid()) {
            // create an index to a property
            const quint64 grandparentRow = parent.internalId();
            // only property carriers have another level of children
            if (grandparentRow == PropertyCarrierIndex &&
                parent.row() >= 0 && uint(parent.row()) < m_propertyCarriers.size()) {
                const IviPropertyCarrier &propCarrier = m_propertyCarriers.at(parent.row());
                if (row >= 0 && uint(row) < propCarrier.iviProperties.size()) {
                    return createIndex(row, column, parent.row());
                }
            }
        } else {
            // create an index to a property carrier
            if (row >= 0 && uint(row) < m_propertyCarriers.size()) {
                return createIndex(row, column, PropertyCarrierIndex);
            }
        }
    }
    return QModelIndex();
}
