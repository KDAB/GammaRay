/*
  qtivipropertymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "baseprobetest.h"
#include "testhelpers.h"

#include <plugins/qtivi/qtivipropertymodel.h>

#include <common/objectbroker.h>
#include <common/objectid.h>

#include <3rdparty/qt/modeltest.h>

#include <QDebug>
#include <QIviServiceObject>
#include <QIviClimateControl>
#if defined(QT_IVIMEDIA_LIB)
#include <QIviAmFmTuner>
#endif

#include <algorithm>

//#define ENABLE_LOG

using namespace GammaRay;
using namespace TestHelpers;

class QtIviPropertyModelTest : public BaseProbeTest
{
    Q_OBJECT
    QIviClimateControl *m_climate;
#if defined(QT_IVIMEDIA_LIB)
    QIviAmFmTuner *m_amfm;
#endif
    QAbstractItemModel *m_model;

    typedef QSharedPointer<QtIviPropertyModel::IviCarrierProperty> IviCarrierProperty;

public:
    explicit QtIviPropertyModelTest(QObject *parent = nullptr)
        : BaseProbeTest(parent)
        , m_climate(nullptr)
#if defined(QT_IVIMEDIA_LIB)
        , m_amfm(nullptr)
#endif
        , m_model(nullptr)
    {
    }

private:
    void createIviProperties()
    {
        m_climate = new QIviClimateControl(QString(), this);
        QVERIFY(m_climate);
        m_climate->setDiscoveryMode(QIviAbstractZonedFeature::LoadOnlySimulationBackends);
        QVERIFY(m_climate->startAutoDiscovery() == QIviAbstractFeature::SimulationBackendLoaded);
        QVERIFY(m_climate->isValid());
#if defined(QT_IVIMEDIA_LIB)
        m_amfm = new QIviAmFmTuner(this);
        QVERIFY(m_amfm);
        m_amfm->setDiscoveryMode(QIviAbstractZonedFeature::LoadOnlySimulationBackends);
        QVERIFY(m_amfm->startAutoDiscovery() == QIviAbstractFeature::SimulationBackendLoaded);
        QVERIFY(m_amfm->isValid());
#endif
    }

    QModelIndex carrierIndex(QObject *carrier, int column) const
    {
        const auto index(m_model->match(m_model->index(0, 0), ObjectModel::ObjectIdRole,
                                        QVariant::fromValue(ObjectId(carrier)), 1,
                                        Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)).value(0));
        return index.sibling(index.row(), column);
    }

    QModelIndex carrierPropertyIndex(QObject *carrier, const QString &property, int column) const
    {
        const QModelIndex parent(carrierIndex(carrier, 0));
        const auto index(parent.isValid()
                         ? m_model->match(m_model->index(0, 0, parent), Qt::DisplayRole, property, 1,
                                          Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)).value(0)
                         : QModelIndex());
        return index.sibling(index.row(), column);
    }

    QVariant carrierPropertyData(QObject *carrier, const QString &property, int column, int role) const
    {
        return carrierPropertyIndex(carrier, property, column).data(role);
    }

    QVariant carrierPropertyValue(QObject *carrier, const QString &property) const
    {
        return carrierPropertyData(carrier, property, QtIviPropertyModel::ValueColumn,
                                   QtIviPropertyModel::RawValue);
    }

    QModelIndex carrierPropertyIndex(QIviProperty *property, int column) const
    {
        const auto index(m_model->match(m_model->index(0, 0), ObjectModel::ObjectIdRole,
                                        QVariant::fromValue(ObjectId(property)), 1,
                                        Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive))
                         .value(0));
        return index.sibling(index.row(), column);
    }

    QVariant carrierPropertyData(QIviProperty *property, int column, int role) const
    {
        return carrierPropertyIndex(property, column).data(role);
    }

    QVariant carrierPropertyValue(QIviProperty *property) const
    {
        return carrierPropertyData(property, QtIviPropertyModel::ValueColumn,
                                   QtIviPropertyModel::RawValue);
    }

    IviCarrierProperty iviCarrierProperty(QObject *carrier, const QString &property) const {
        if (!carrier)
            return IviCarrierProperty::create();

        const QMetaObject *mo(carrier->metaObject());
        const int index(mo->indexOfProperty(qPrintable(property)));

        if (index == -1)
            return IviCarrierProperty::create();

        const QMetaProperty metaProperty(mo->property(index));

        if (metaProperty.userType() == qMetaTypeId<QIviProperty *>()) {
            return IviCarrierProperty::create(metaProperty.read(carrier).value<QIviProperty *>(), metaProperty);
        }

        return IviCarrierProperty::create(metaProperty);
    }

    // Check ivi object properties vs model properties
    bool testIviObject(QObject *iviObject) const
    {
#if defined(ENABLE_LOG)
        qWarning() << "Testing ivi object: " << iviObject;
#endif
        QVERIFY_RETURN_FALSE(iviObject);

        QVector<QString> iviPropertyNames;
        QMap<QString, QVariant> iviProperties;
        const QMetaObject *iviMo(iviObject->metaObject());
        int propertyOffset = -1;

        if (qobject_cast<QIviServiceObject *>(iviObject))
            propertyOffset = QIviServiceObject::staticMetaObject.propertyOffset();
        else if (qobject_cast<QIviAbstractFeature *>(iviObject))
            propertyOffset = QIviAbstractFeature::staticMetaObject.propertyOffset();

        QVERIFY_RETURN_FALSE(propertyOffset != -1);

        for (int i = propertyOffset; i < iviMo->propertyCount(); ++i) {
            const QMetaProperty property(iviMo->property(i));

            iviPropertyNames << QLatin1String(property.name());
            QVERIFY_RETURN_FALSE(!iviPropertyNames.last().isEmpty());

            if (property.userType() == qMetaTypeId<QIviProperty *>()) {
                QIviProperty *ivi = property.read(iviObject).value<QIviProperty *>();
                QVERIFY_RETURN_FALSE(ivi);
                iviProperties[iviPropertyNames.last()] = ivi->value();
            } else {
                iviProperties[iviPropertyNames.last()] = property.read(iviObject);
            }
        }

        std::stable_sort(iviPropertyNames.begin(), iviPropertyNames.end());
        QVERIFY_RETURN_FALSE(!iviPropertyNames.isEmpty());
        QVERIFY_RETURN_FALSE(!iviProperties.isEmpty());

        const QModelIndex carrierIndex(this->carrierIndex(iviObject, 0));
        const int carrierPropertyCount(m_model->rowCount(carrierIndex));
        QVERIFY_RETURN_FALSE(carrierIndex.isValid());
        QCOMPARE_RETURN_FALSE(iviProperties.count(), carrierPropertyCount);

        QVector<QString> carrierModelPropertyNames;

        for (int i = 0; i < carrierPropertyCount; ++i) {
            const QModelIndex nameIndex(m_model->index(i, QtIviPropertyModel::NameColumn, carrierIndex));
            carrierModelPropertyNames << nameIndex.data(Qt::DisplayRole).toString();
            QVERIFY_RETURN_FALSE(!carrierModelPropertyNames.last().isEmpty());
            QCOMPARE_RETURN_FALSE(iviProperties[carrierModelPropertyNames.last()],
                    carrierPropertyValue(iviObject, carrierModelPropertyNames.last()));
        }

        std::stable_sort(carrierModelPropertyNames.begin(), carrierModelPropertyNames.end());
        QCOMPARE_RETURN_FALSE(carrierModelPropertyNames.count(), iviPropertyNames.count());
        QVERIFY_RETURN_FALSE(carrierModelPropertyNames == iviPropertyNames);

        if (QIviAbstractZonedFeature *zonedFeature = qobject_cast<QIviAbstractZonedFeature *>(iviObject)) {
            foreach (QIviAbstractZonedFeature *subZone, zonedFeature->zones()) {
                if (!testIviObject(subZone)) {
                    return false;
                }
            }
        }

        return true;
    }

private slots:
    void initTestCase()
    {
        createIviProperties();
        createProbe();
        m_model = ObjectBroker::model("com.kdab.GammaRay.PropertyModel");
        QVERIFY(m_model);
    }

    void testModelContent()
    {
        QVERIFY(new ModelTest(m_model, this));
        QVERIFY(testIviObject(m_climate));
#if defined(QT_IVIMEDIA_LIB)
        QVERIFY(testIviObject(m_amfm));
#endif
    }

    void testModelContentChanges_data()
    {
        QTest::addColumn<QObject *>("carrier");
        QTest::addColumn<QString>("property");
        QTest::addColumn<bool>("override");
        QTest::addColumn<QVariant>("value");
        QTest::addColumn<QVariant>("expect");
        QTest::addColumn<bool>("invalidProperty");

        // Invalid value mean nothing to set
        // Invalid expect mean use value

        QTest::newRow("climate-default-climateMode")
                << (QObject *)m_climate
                << "climateMode"
                << true
                << QVariant()
                << QVariant::fromValue(QIviClimateControl::ClimateOn)
                << false
        ;

        QTest::newRow("climate-climateMode-Off")
                << (QObject *)m_climate
                << "climateMode"
                << false
                << QVariant::fromValue(QIviClimateControl::ClimateOff)
                << QVariant()
                << false
        ;

        QTest::newRow("climate-airflowDirections-Floor")
                << (QObject *)m_climate
                << "airflowDirections"
                << false
                << QVariant::fromValue(QIviClimateControl::AirflowDirections(QIviClimateControl::Floor))
                << QVariant()
                << false
        ;

        QTest::newRow("climate-airflowDirections-Floor|Dashboard")
                << (QObject *)m_climate
                << "airflowDirections"
                << false
                << QVariant::fromValue(QIviClimateControl::AirflowDirections(QIviClimateControl::Floor |
                                                                             QIviClimateControl::Dashboard))
                << QVariant()
                << false
        ;

        QTest::newRow("climate-FrontLeft-targetTemperature-25")
                << (QObject *)m_climate->zoneAt("FrontLeft")
                << "targetTemperature"
                << false
                << QVariant(25)
                << QVariant()
                << false
        ;

        QTest::newRow("climate-FrontRight-seatHeater-8")
                << (QObject *)m_climate->zoneAt("FrontRight")
                << "seatHeater"
                << false
                << QVariant(8)
                << QVariant()
                << false
        ;

        QTest::newRow("unknown-property")
                << (QObject *)nullptr
                << "hey"
                << false
                << QVariant(42)
                << QVariant()
                << true
        ;

        QTest::newRow("climate-unknown-property-override")
                << (QObject *)m_climate->zoneAt("FrontLeft")
                << "hey"
                << true
                << QVariant(42)
                << QVariant()
                << true
        ;

        QTest::newRow("climate-FrontLeft-heater-true(readonly)")
                << (QObject *)m_climate->zoneAt("FrontLeft")
                << "heater"
                << false
                << QVariant(true)
                << QVariant(false)
                << false
        ;

#if defined(QT_IVIMEDIA_LIB)
        QTest::newRow("amfm-default-discoveryMode")
                << (QObject *)m_amfm
                << "discoveryMode"
                << false
                << QVariant()
                << QVariant::fromValue(QIviAbstractFeature::LoadOnlySimulationBackends)
                << false
        ;

        QTest::newRow("amfm-discoveryMode-AutoDiscovery")
                << (QObject *)m_amfm
                << "discoveryMode"
                << false
                << QVariant::fromValue(QIviAbstractFeature::AutoDiscovery)
                << QVariant()
                << false
        ;

        QTest::newRow("amfm-band-AMBand")
                << (QObject *)m_amfm
                << "band"
                << false
                << QVariant::fromValue(QIviAmFmTuner::AMBand)
                << QVariant()
                << false
        ;

        QTest::newRow("amfm-unknown-property-override")
                << (QObject *)m_amfm
                << "hey"
                << true
                << QVariant(42)
                << QVariant()
                << true
        ;

        QTest::newRow("amfm-scanRunning-true(readonly)")
                << (QObject *)m_amfm
                << "scanRunning"
                << false
                << QVariant(true)
                << QVariant(false)
                << false
        ;
#endif
    }

    void testModelContentChanges()
    {
        QFETCH(QObject *, carrier);
        QFETCH(QString, property);
        QFETCH(bool, override);
        QFETCH(QVariant, value);
        QFETCH(QVariant, expect);
        QFETCH(bool, invalidProperty);
        const QVariant expectValue(expect.isValid() ? expect : value);
        const IviCarrierProperty iviProperty(iviCarrierProperty(carrier, property));

        if (invalidProperty) {
            QEXPECT_FAIL("", "Expected not found property", Continue);
        }
        QVERIFY(iviProperty->isValid());

        const QModelIndex valueIndex(carrierPropertyIndex(carrier, property, QtIviPropertyModel::ValueColumn));
        if (iviProperty->m_iviProperty) {
            QCOMPARE(valueIndex, carrierPropertyIndex(iviProperty->m_iviProperty, valueIndex.column()));
        }
        QCOMPARE(valueIndex.isValid(), iviProperty->isValid());
        QCOMPARE(!invalidProperty, iviProperty->isValid());


#if defined(ENABLE_LOG)
        qWarning("Original value: %s", invalidProperty ? "" : qPrintable(iviProperty->value().toString()));
#endif
        const QVariant originalValue = iviProperty->cppValue(carrier);
        const QModelIndex overrideIndex(valueIndex.sibling(valueIndex.row(), QtIviPropertyModel::OverrideColumn));

#if defined(ENABLE_LOG)
        qWarning("Setting override: %i", (int)override);
#endif
        const bool willOverride(iviProperty->isAvailable() && iviProperty->isOverridable() &&
                                (overrideIndex.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked) != override);
        QCOMPARE(m_model->setData(overrideIndex, override ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole), willOverride);
        QCOMPARE((overrideIndex.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked),
                 (!iviProperty->isAvailable() ? false : override));

        if (value.isValid()) {
#if defined(ENABLE_LOG)
            qWarning("Setting value: %s", qPrintable(value.toString()));
#endif
            const bool willWrite(iviProperty->isAvailable() && iviProperty->isWritable() &&
                                 valueIndex.data(QtIviPropertyModel::RawValue) != value);
            QCOMPARE(m_model->setData(valueIndex, value, Qt::EditRole), willWrite);
            if (willWrite) {
                QCOMPARE(valueIndex.data(QtIviPropertyModel::RawValue), value);
            }
        }

        if (iviProperty->isValid()) {
            QCOMPARE(iviProperty->cppValue(carrier), iviProperty->isAvailable() && !override ? expectValue : originalValue);
        }
        QCOMPARE(iviProperty->isAvailable(),
                valueIndex.flags().testFlag(Qt::ItemIsEnabled));
        QCOMPARE(iviProperty->isAvailable() && iviProperty->isWritable(),
                 valueIndex.flags().testFlag(Qt::ItemIsEditable));
        QCOMPARE(valueIndex.data(QtIviPropertyModel::RawValue),
                 invalidProperty ? QVariant() : expectValue);
    }
};

QTEST_MAIN(QtIviPropertyModelTest)

#include "qtivipropertymodeltest.moc"
