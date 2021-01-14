/*
  qtiviobjectmodel.cpp

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
#include <private/qiviabstractfeature_p.h>
#include <private/qividefaultpropertyoverrider_p.h>

#include <QThread>
#include <QItemSelectionModel>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMutexLocker>

#include <QJsonDocument>
#include <QJsonArray>

#include <iostream>

static const quintptr PropertyCarrierIndex = ~quintptr(0);

//#define IF_DEBUG(x) (x)
#define IF_DEBUG(x)

using namespace GammaRay;
QT_USE_NAMESPACE

class QIviGammarayPropertyOverrider : public QIviDefaultPropertyOverrider
{
public:
    explicit QIviGammarayPropertyOverrider(QIviAbstractFeature *carrier, QObject *parent = nullptr);

    bool setOverridenValue(int index, const QVariant &value) override;
    QString displayTextAt(int index) const override;
    QVariant editValueAt(int index) const override;
    QVariant iviConstraintsAt(int index) const override;
};


QIviGammarayPropertyOverrider::QIviGammarayPropertyOverrider(QIviAbstractFeature *carrier, QObject *parent)
    : QIviDefaultPropertyOverrider(carrier, parent)
{

}

bool QIviGammarayPropertyOverrider::setOverridenValue(int index, const QVariant &value)
{
    if (m_carriers.empty())
        return {};

    const PropertyOverride &property = m_properties.at(index);

    QVariant toSet = value;
    if (value.userType() == qMetaTypeId<EnumValue>()) {
        QVariant typeReference = property.cppValue();
        if (typeReference.type() == QVariant::Int) {
            toSet = value.value<EnumValue>().value();
        } else {
            *(static_cast<int*>(typeReference.data())) = value.value<EnumValue>().value();
            toSet = typeReference;
        }
    }

    return QIviDefaultPropertyOverrider::setOverridenValue(index, toSet);
}

QString QIviGammarayPropertyOverrider::displayTextAt(int index) const
{
    if (m_carriers.empty())
        return {};

    const PropertyOverride &property = m_properties.at(index);
    const QVariant value = property.cppValue();
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QString enumStr(EnumUtil::enumToString(value, nullptr, mo));
    if (!enumStr.isEmpty())
        return enumStr;
    return VariantHandler::displayString(value);
}

QVariant QIviGammarayPropertyOverrider::editValueAt(int index) const
{
    if (m_carriers.empty())
        return {};

    const PropertyOverride &property = m_properties.at(index);
    const QVariant value = property.cppValue();
    const QMetaObject *const mo(QMetaType::metaObjectForType(value.userType()));
    const QMetaEnum me(EnumUtil::metaEnum(value, nullptr, mo));
    if (me.isValid()) {
        const int num(EnumUtil::enumToInt(value, me));
        return QVariant::fromValue(EnumRepositoryServer::valueFromMetaEnum(num, me));
    }
    return VariantHandler::serializableVariant(value);
}

QVariant QIviGammarayPropertyOverrider::iviConstraintsAt(int index) const
{
    if (m_carriers.empty())
        return {};

    auto carrier = m_carriers.front();
    const PropertyOverride &property = m_properties.at(index);

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

    if (!loadDoc.object().contains(property.name()))
        return {};

    QJsonValue constraints = loadDoc.object().value(property.name());
    QLatin1String range("range");
    if (constraints.toObject().contains(range)) {
        QJsonArray vals = constraints.toObject().value(range).toArray();
        return QVariantList() << static_cast<uint>(QtIviObjectModel::RangeConstraints) << vals.at(0).toDouble() << vals.at(1).toDouble();
    }
    QVariantList result;
    QLatin1String minimum("minimum");
    if (constraints.toObject().contains(minimum)) {
        double val = constraints.toObject().value(minimum).toDouble();
        result << static_cast<uint>(QtIviObjectModel::MinRangeConstraints) << val;
    }
    QLatin1String maximum("maximum");
    if (constraints.toObject().contains(maximum)) {
        double val = constraints.toObject().value(maximum).toDouble();
        result << static_cast<uint>(QtIviObjectModel::MaxRangeConstraints) << val;
    }
    if (!result.isEmpty())
        return result;
    QLatin1String domain("domain");
    if (constraints.toObject().contains(domain)) {
        const QVariantList vals = constraints.toObject().value(domain).toArray().toVariantList();
        QVariantList res;
        res << static_cast<uint>(QtIviObjectModel::AvailableValuesConstraints) << vals;
        return res;
    }
    return {};
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
            sz.m_service = std::make_shared<QIviGammarayPropertyOverrider>(featureObj);
            m_serviceCarriers.push_back(sz);
            endInsertRows();

            const QMetaObject *mo(obj->metaObject());
            const int propertyOffset = QIviAbstractFeature::staticMetaObject.propertyCount();
            const int propertyCount(mo->propertyCount() - propertyOffset);

            if (propertyCount > 0) {
                const int featureRow(m_serviceCarriers.size() - 1);

                const QModelIndex featureIndex(createIndex(featureRow, 0, -1));
                beginInsertRows(featureIndex, 0, propertyCount - 1);
                sz.m_service->addCarrier(featureObj);
                endInsertRows();

                connect(sz.m_service.get(), &QIviDefaultPropertyOverrider::propertyChanged, this, &QtIviObjectModel::propertyChanged);
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
        const QIviDefaultPropertyOverriderPtr &carrier(m_serviceCarriers.at(index.parent().row()).m_service);

        // We have two levels, this is the second
        flags |= Qt::ItemNeverHasChildren;

        switch (index.column()) {
        case ValueColumn: {
            if (carrier->isWritableAt(index.row()) || carrier->isOverridableAt(index.row()))
                flags |= Qt::ItemIsEditable;
            break;
        }

        case WritableColumn: {
            flags |= Qt::ItemIsUserCheckable;
            if (carrier->isOverriddenAt(index.row()) && !carrier->isWritableAt(index.row()))
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEnabled;
            break;
        }

        case OverrideColumn: {
            flags |= Qt::ItemIsUserCheckable;
            if (carrier->isOverridableAt(index.row()) || carrier->isOverriddenAt(index.row()))
                flags |= Qt::ItemIsEditable;
            else
                flags &= ~Qt::ItemIsEnabled;
            break;
        }

        default:
            break;
        }

        if (!carrier->isAvailableAt(index.row())) {
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

    const auto parentRow = index.internalId();
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

//            case ObjectModel::ObjectIdRole:
//                return QVariant::fromValue(carrier->objectId());

            default:
                break;
            }
        }
    } else {
        // Property

        if (parentRow < m_serviceCarriers.size()) {
            const QIviDefaultPropertyOverriderPtr &carrier = m_serviceCarriers.at(parentRow).m_service;

            if (index.row() >= 0 && index.row() < carrier->propertyCount()) {
                switch (role) {
                case Qt::DisplayRole: {
                    switch (index.column()) {
                    case NameColumn:
                        return carrier->nameAt(index.row());
                    case ValueColumn:
                        return carrier->displayTextAt(index.row());
                    case TypeColumn: {
                        QString result = carrier->typeNameAt(index.row());
                        QVariant v = carrier->iviConstraintsAt(index.row());
                        if (v.isValid() && v.type() == QVariant::List)
                            result += QLatin1String(" (!)");
                        return result;
                    }
                    default:
                        break;
                    }

                    break;
                }

                case Qt::EditRole: {
                    switch (index.column()) {
                    case ValueColumn:
                        return carrier->editValueAt(index.row());
                    default:
                        break;
                    }

                    break;
                }

                case Qt::CheckStateRole: {
                    switch (index.column()) {
                    case WritableColumn:
                        return carrier->isWritableAt(index.row()) || carrier->isOverriddenAt(index.row()) ? Qt::Checked : Qt::Unchecked;
                    case OverrideColumn:
                        return carrier->isOverriddenAt(index.row()) ? Qt::Checked : Qt::Unchecked;
                    default:
                        break;
                    }

                    break;
                }

                case Qt::ToolTipRole: {
                    switch (index.column()) {
                    case TypeColumn:
                    case ValueColumn: {
                        QVariant v = carrier->iviConstraintsAt(index.row());
                        if (!v.isValid() || v.type() != QVariant::List)
                            return  {};
                        QVariantList vl = v.toList();
                        QString result;
                        while (!vl.isEmpty()) {
                            switch (vl.front().toUInt()) {
                            case QtIviObjectModel::RangeConstraints:
                                result += QString(QLatin1String("Range: [%1 - %2]\n")).arg(vl.at(1).toDouble()).arg(vl.at(2).toDouble());
                                vl.pop_front();
                                vl.pop_front();
                                vl.pop_front();
                                break;
                            case QtIviObjectModel::MinRangeConstraints:
                                result += QString(QLatin1String("Minimum: %1\n")).arg(vl.at(1).toDouble());
                                vl.pop_front();
                                vl.pop_front();
                                break;
                            case QtIviObjectModel::MaxRangeConstraints:
                                result += QString(QLatin1String("Maximum: %1\n")).arg(vl.at(1).toDouble());
                                vl.pop_front();
                                vl.pop_front();
                                break;
                            case QtIviObjectModel::AvailableValuesConstraints: {
                                vl.pop_front();
                                QStringList valstrings;
                                for(const auto &vi: qAsConst(vl))
                                    valstrings << vi.toString();
                                result += QString(QLatin1String("Valid Values: [%1]\n")).arg(valstrings.join(QLatin1String(", ")));
                                vl.clear();
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        return result.trimmed();
                    }
                    default:
                        break;
                    }

                    break;
                }

                case IsIviProperty: {
                    return carrier->isOverridableAt(index.row());
                }

                case ValueConstraintsRole: {
                    switch (index.column()) {
                    case ValueColumn:
                        return carrier->iviConstraintsAt(index.row());
                    default:
                        break;
                    }

                    break;
                }

//                case ObjectModel::ObjectIdRole:
//                    return QVariant::fromValue(property.objectId());

//                case RawValue:
//                    return property.cppValue();
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
    const auto parentRow = index.internalId();
    if (!index.isValid() || parentRow == PropertyCarrierIndex ||
            parentRow >= m_serviceCarriers.size() || !flags(index).testFlag(Qt::ItemIsEditable)) {
        return false;
    }

    QIviDefaultPropertyOverriderPtr &carrier = m_serviceCarriers[parentRow].m_service;

    if (index.row() >= 0 && index.row() < carrier->propertyCount()) {

        switch (index.column()) {
        case ValueColumn: {
            if (role == Qt::DisplayRole || role == Qt::EditRole) {
                if (carrier->setOverridenValue(index.row(), value)) {
                    if (!carrier->hasNotifySignalAt(index.row())) {
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
                if (!carrier->isWritableAt(index.row()) && carrier->isOverriddenAt(index.row())) {
                    carrier->setOverride(index.row(), false);
                    emitRowDataChanged(index);
                    return true;
                }
            }

            break;
        }

        case OverrideColumn: {
            if (role == Qt::CheckStateRole) {
                const bool isOverride = value.value<Qt::CheckState>() == Qt::Checked;

                if(carrier->setOverride(index.row(), isOverride)) {
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

void QtIviObjectModel::propertyChanged()
{
    if (QIviDefaultPropertyOverrider * carrier = qobject_cast<QIviDefaultPropertyOverrider *>(sender())) {
        // A plain Qt property changed in a service or feature
        // Let's update the complete children as we can not known the property that changed

        for (int row = 0; row < int(m_serviceCarriers.size()); ++row) {
            if (m_serviceCarriers.at(row).m_service.get() == carrier) {
                const QModelIndex parent(index(row, 0));
                const int ccount(columnCount(parent));
                const int rcount(rowCount(parent));
                if (ccount > 0 && rcount > 0) {
                    emit dataChanged(index(0, 0, parent), index(rcount - 1, ccount - 1, parent));
                }
                break;
            }
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
            const QIviDefaultPropertyOverriderPtr &carrier = m_serviceCarriers.at(parent.row()).m_service;
            return carrier->propertyCount();
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
        const auto parentRow = child.internalId();
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
            const auto grandparentRow = parent.internalId();
            // only carriers have another level of children
            if (grandparentRow == PropertyCarrierIndex &&
                parent.row() >= 0 && uint(parent.row()) < m_serviceCarriers.size()) {
                const QIviDefaultPropertyOverriderPtr &carrier = m_serviceCarriers.at(parent.row()).m_service;
                if (row >= 0 && row < carrier->propertyCount()) {
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
