/*
  qtivipropertymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include <plugins/qtivi/qtivipropertymodel.h>

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/objectbroker.h>
#include <common/objectid.h>

#include <3rdparty/qt/modeltest.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QIviClimateControl>

//#define ENABLE_LOG

#define QVERIFY_RETURN_FALSE(statement) \
do {\
    if (!QTest::qVerify(static_cast<bool>(statement), #statement, "", __FILE__, __LINE__))\
    return false;\
} while (0)

#define QCOMPARE_RETURN_FALSE(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
    return false;\
} while (0)

using namespace GammaRay;

class QtIviPropertyModelTest : public QObject
{
    Q_OBJECT
    QIviClimateControl *m_climate;
    QAbstractItemModel *m_model;

public:
    explicit QtIviPropertyModelTest(QObject *parent = nullptr)
        : QObject(parent)
        , m_climate(nullptr)
        , m_model(nullptr)
    { }

private:
    void createIviProperties()
    {
        m_climate = new QIviClimateControl(QString(), this);
        QVERIFY(m_climate);
        m_climate->setDiscoveryMode(QIviAbstractZonedFeature::LoadOnlyProductionBackends);
        QVERIFY(m_climate->startAutoDiscovery() == QIviAbstractFeature::ProductionBackendLoaded);
        QVERIFY(m_climate->isValid());
    }

    void createProbe()
    {
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        QVERIFY(new ProbeCreator(ProbeCreator::Create));
        QTest::qWait(1); // event loop re-entry
    }

    template <typename T>
    QVariant iviValue(const T &value) const
    {
        return std::is_same<typename std::decay<T>::type, QVariant>::value
                ? QtIviPropertyOverrider::iviValue(value, value.userType())
                : QtIviPropertyOverrider::iviValue(QVariant::fromValue(value), qMetaTypeId<T>())
        ;
    }

    QModelIndex zonedFeatureIndex(const QString &zone, int column) const
    {
        const auto index(m_model->match(m_model->index(0, 0), QtIviPropertyModel::ZoneName,
                                        zone, 1,
                                        Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)).value(0));
        return index.sibling(index.row(), column);
    }

    QModelIndex zonedFeaturePropertyIndex(const QString &zone, const QString &property, int column) const
    {
        const QModelIndex parent(zonedFeatureIndex(zone, 0));
        const auto index(parent.isValid()
                         ? m_model->match(m_model->index(0, 0, parent), Qt::DisplayRole, property, 1,
                                          Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap)).value(0)
                         : QModelIndex());
        return index.sibling(index.row(), column);
    }

    QVariant zonedFeaturePropertyData(const QString &zone, const QString &property, int column, int role) const
    {
        return zonedFeaturePropertyIndex(zone, property, column).data(role);
    }

    QVariant zonedFeaturePropertyIviData(const QString &zone, const QString &property) const
    {
        return zonedFeaturePropertyIndex(zone, property, QtIviPropertyModel::ValueColumn)
                .data(QtIviPropertyModel::NativeIviValue);
    }

    template <typename T>
    T zonedFeaturePropertyData(const QString &zone, const QString &property, int column, int role) const
    {
        return zonedFeaturePropertyIndex(zone, property, column).data(role).value<T>();
    }

    QModelIndex zonedFeaturePropertyIndex(QIviProperty *property, int column) const
    {
        const auto index(m_model->match(m_model->index(0, 0), QtIviPropertyModel::ObjectIdRole,
                                        QVariant::fromValue(ObjectId(property)), 1,
                                        Qt::MatchFlags(Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive))
                         .value(0));
        return index.sibling(index.row(), column);
    }

    QVariant zonedFeaturePropertyData(QIviProperty *property, int column, int role) const
    {
        return zonedFeaturePropertyIndex(property, column).data(role);
    }

    QVariant zonedFeaturePropertyIviData(QIviProperty *property) const
    {
        return zonedFeaturePropertyIndex(property, QtIviPropertyModel::ValueColumn)
                .data(QtIviPropertyModel::NativeIviValue);
    }

    template <typename T>
    T zonedFeaturePropertyData(QIviProperty *property, int column, int role) const
    {
        return zonedFeaturePropertyIndex(property, column).data(role).value<T>();
    }

    // Check zoned feature properties vs model properties
    bool testZonedFeature(const QIviAbstractZonedFeature *const zonedFeature) const
    {
#if defined(ENABLE_LOG)
        qWarning("Testing zone: %s/%s", zonedFeature->metaObject()->className(),
                 zonedFeature->zone().toLocal8Bit().constData());
#endif
        QVERIFY_RETURN_FALSE(zonedFeature);

        QVector<QString> zonedFeaturePropertyNames;
        QMap<QString, QIviProperty *> zonedFeatureProperties;
        const QMetaObject *mo(zonedFeature->metaObject());

        for (int i = 0; i < mo->propertyCount(); ++i) {
            const QMetaProperty property(mo->property(i));

            if (property.userType() == qMetaTypeId<QIviProperty *>()) {
                zonedFeaturePropertyNames << QLatin1String(property.name());
                QVERIFY_RETURN_FALSE(!zonedFeaturePropertyNames.last().isEmpty());
                zonedFeatureProperties[zonedFeaturePropertyNames.last()] =
                        property.read(zonedFeature).value<QIviProperty *>();
                QVERIFY_RETURN_FALSE(zonedFeatureProperties[zonedFeaturePropertyNames.last()]);
            }
        }

        qStableSort(zonedFeaturePropertyNames);
        QVERIFY_RETURN_FALSE(!zonedFeaturePropertyNames.isEmpty());
        QVERIFY_RETURN_FALSE(!zonedFeatureProperties.isEmpty());

        const QModelIndex zonedFeatureIndex(this->zonedFeatureIndex(zonedFeature->zone(), 0));
        const int zonedFeaturePropertyCount(m_model->rowCount(zonedFeatureIndex));
        QVERIFY_RETURN_FALSE(zonedFeatureIndex.isValid());
        QCOMPARE_RETURN_FALSE(zonedFeatureProperties.count(), zonedFeaturePropertyCount);

        QVector<QString> zonedFeatureModelPropertyNames;

        for (int i = 0; i < zonedFeaturePropertyCount; ++i) {
            const QModelIndex nameIndex(m_model->index(i, QtIviPropertyModel::NameColumn, zonedFeatureIndex));
            zonedFeatureModelPropertyNames << nameIndex.data(Qt::DisplayRole).toString();
            QVERIFY_RETURN_FALSE(!zonedFeatureModelPropertyNames.last().isEmpty());
            QCOMPARE_RETURN_FALSE(zonedFeatureProperties[zonedFeatureModelPropertyNames.last()]->value(),
                    nameIndex.sibling(i, QtIviPropertyModel::ValueColumn).data(QtIviPropertyModel::NativeIviValue));
            QCOMPARE_RETURN_FALSE(zonedFeatureProperties[zonedFeatureModelPropertyNames.last()]->isAvailable(),
                    nameIndex.flags().testFlag(Qt::ItemIsEnabled));
        }

        qStableSort(zonedFeatureModelPropertyNames);
        QCOMPARE_RETURN_FALSE(zonedFeatureModelPropertyNames.count(), zonedFeaturePropertyNames.count());
        QVERIFY_RETURN_FALSE(zonedFeatureModelPropertyNames == zonedFeaturePropertyNames);

        foreach (const QIviAbstractZonedFeature *subZone, zonedFeature->zones()) {
            if (!testZonedFeature(subZone)) {
                return false;
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
        QVERIFY(testZonedFeature(m_climate));
    }

    void testModelContentChanges_data()
    {
        QTest::addColumn<QString>("zone");
        QTest::addColumn<QString>("property");
        QTest::addColumn<bool>("override");
        QTest::addColumn<QVariant>("value");
        QTest::addColumn<QVariant>("expect");
        QTest::addColumn<bool>("invalidProperty");

        // Invalid value mean nothing to set
        // Invalid expect mean use value

        QTest::newRow("default-climateMode")
                << QString()
                << "climateMode"
                << true
                << QVariant()
                << QVariant::fromValue(QIviClimateControl::ClimateOn)
                << false
        ;

        QTest::newRow("climateMode-Off")
                << QString()
                << "climateMode"
                << false
                << QVariant::fromValue(QIviClimateControl::ClimateOff)
                << QVariant()
                << false
        ;

        QTest::newRow("airflowDirections-Floor")
                << QString()
                << "airflowDirections"
                << false
                << QVariant::fromValue(QIviClimateControl::AirflowDirections(QIviClimateControl::Floor))
                << QVariant()
                << false
        ;

        QTest::newRow("airflowDirections-Floor|Dashboard")
                << QString()
                << "airflowDirections"
                << false
                << QVariant::fromValue(QIviClimateControl::AirflowDirections(QIviClimateControl::Floor | QIviClimateControl::Dashboard))
                << QVariant()
                << false
        ;

        QTest::newRow("FrontLeft-targetTemperature-25")
                << "FrontLeft"
                << "targetTemperature"
                << false
                << QVariant(25)
                << QVariant()
                << false
        ;

        QTest::newRow("FrontRight-seatHeater-8")
                << "FrontRight"
                << "seatHeater"
                << false
                << QVariant(8)
                << QVariant()
                << false
        ;

        QTest::newRow("unknown-property")
                << ""
                << "hey"
                << false
                << QVariant(42)
                << QVariant()
                << true
        ;

        QTest::newRow("unknown-property-override")
                << "FrontLeft"
                << "hey"
                << true
                << QVariant(42)
                << QVariant()
                << true
        ;

        QTest::newRow("FrontLeft-heater-true(readonly)")
                << "FrontLeft"
                << "heater"
                << false
                << QVariant(true)
                << QVariant(false)
                << false
        ;
    }

    void testModelContentChanges()
    {
        QFETCH(QString, zone);
        QFETCH(QString, property);
        QFETCH(bool, override);
        QFETCH(QVariant, value);
        QFETCH(QVariant, expect);
        QFETCH(bool, invalidProperty);
        const QVariant expectValue(expect.isValid() ? expect : value);
        const QVariant expectIviValue(this->iviValue(expectValue));
        const QVariant iviValue(this->iviValue(value));

        QIviProperty *iviProperty((zone.isEmpty() ? m_climate : m_climate->zoneAt(zone))
                                  ->property(qPrintable(property)).value<QIviProperty *>());
        if (invalidProperty) {
            QEXPECT_FAIL("", "Expected not found property", Continue);
        }
        QVERIFY(iviProperty);

        const QModelIndex valueIndex(zonedFeaturePropertyIndex(zone, property, QtIviPropertyModel::ValueColumn));
        QCOMPARE(valueIndex, zonedFeaturePropertyIndex(iviProperty, valueIndex.column()));

#if defined(ENABLE_LOG)
        qWarning("Original value: %s", invalidProperty ? "" : qPrintable(iviProperty->value().toString()));
#endif
        const QVariant originalValue = invalidProperty ? QVariant() : iviProperty->value();
        const QModelIndex overrideIndex(valueIndex.sibling(valueIndex.row(), QtIviPropertyModel::OverrideColumn));

#if defined(ENABLE_LOG)
        qWarning("Setting override: %i", (int)override);
#endif
        const bool needOverride((overrideIndex.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked) != override);
        QCOMPARE(overrideIndex.isValid(), !invalidProperty);
        QCOMPARE(m_model->setData(overrideIndex, override ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole),
                 (invalidProperty || !iviProperty->isAvailable() ? false : needOverride));
        QCOMPARE((overrideIndex.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked),
                 (invalidProperty || !iviProperty->isAvailable() ? false : override));

        if (value.isValid()) {
#if defined(ENABLE_LOG)
            qWarning("Setting value: %s", qPrintable(value.toString()));
#endif
            const bool needValueUpdate(invalidProperty || !iviProperty->isAvailable() ? false : valueIndex.data(QtIviPropertyModel::NativeIviValue) != iviValue);
            QCOMPARE(m_model->setData(valueIndex, iviValue, Qt::EditRole), needValueUpdate);
        }

        if (!invalidProperty) {
            QCOMPARE(iviProperty->value(), iviProperty->isAvailable() && !override ? expectIviValue : originalValue);
        }
        QCOMPARE(invalidProperty ? false : iviProperty->isAvailable(),
                valueIndex.flags().testFlag(Qt::ItemIsEnabled));
        QCOMPARE(valueIndex.data(QtIviPropertyModel::NativeIviValue),
                 invalidProperty ? QVariant() : expectIviValue);
    }
};

QTEST_MAIN(QtIviPropertyModelTest)

#include "qtivipropertymodeltest.moc"
