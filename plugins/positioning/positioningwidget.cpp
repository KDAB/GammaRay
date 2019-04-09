/*
  positioningwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "positioningwidget.h"
#include "ui_positioningwidget.h"
#include "positioningclient.h"
#include "mapcontroller.h"

#include <ui/propertybinder.h>
#include <common/objectbroker.h>

#include <QAction>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QNmeaPositionInfoSource>
#include <QQuickWidget>
#include <QQmlContext>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
Q_DECLARE_METATYPE(QGeoPositionInfo)
#endif

using namespace GammaRay;

static QObject *createPositioningClient(const QString &name, QObject *parent)
{
    Q_UNUSED(name);
    return new PositioningClient(parent);
}

PositioningWidget::PositioningWidget(QWidget* parent):
    QWidget(parent),
    ui(new Ui::PositioningWidget),
    m_mapController(new MapController(this)),
    m_replaySource(nullptr),
    m_updateLock(false)
{
    ui->setupUi(this);
    auto mapView = new QQuickWidget;
    mapView->rootContext()->setContextProperty(QStringLiteral("_controller"), m_mapController);
    ui->topLayout->addWidget(mapView);

    ObjectBroker::registerClientObjectFactoryCallback<PositioningInterface*>(createPositioningClient);

    m_interface = ObjectBroker::object<PositioningInterface*>();
    Q_ASSERT(m_interface);
    connect(m_interface, &PositioningInterface::positionInfoChanged, this, [this]() {
        m_mapController->setSourceCoordinate(m_interface->positionInfo().coordinate());
        m_mapController->setSourceDirection(m_interface->positionInfo().attribute(QGeoPositionInfo::Direction));
        m_mapController->setSourceHorizontalAccuracy(m_interface->positionInfo().attribute(QGeoPositionInfo::HorizontalAccuracy));
        if (ui->overrideBox->isChecked())
            return;
        setUiToPosition(m_interface->positionInfo());
    });

    connect(ui->overrideBox, &QCheckBox::toggled, this, [this](bool enabled) {
        if (enabled && m_interface->positionInfoOverride().isValid()) {
            setUiToPosition(m_interface->positionInfoOverride());
        } else if (!enabled && m_interface->positionInfo().isValid()) {
            setUiToPosition(m_interface->positionInfo());
        }
        updateWidgetState();
        updatePosition();
    });
    connect(ui->latitude, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->longitude, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->horizontalSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->horizontalAccuracy, QOverload<int>::of(&QSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->altitude, QOverload<int>::of(&QSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->verticalSpeed, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->verticalAccuracy, QOverload<int>::of(&QSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->direction, QOverload<int>::of(&QSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->magneticVariation, QOverload<int>::of(&QSpinBox::valueChanged), this, &PositioningWidget::updatePosition);
    connect(ui->timestamp, &QDateTimeEdit::dateTimeChanged, this, &PositioningWidget::updatePosition);

    connect(m_mapController, &MapController::overrideCoordinateChanged, this, [this]() {
        if (m_updateLock)
            return;
        m_updateLock = true;
        ui->latitude->setValue(m_mapController->overrideCoordinate().latitude());
        ui->longitude->setValue(m_mapController->overrideCoordinate().longitude());
        ui->timestamp->setDateTime(QDateTime::currentDateTime());
        m_updateLock = false;
        updatePosition();
    });
    connect(m_mapController, &MapController::overrideDirectionChanged, this, [this]() {
        if (m_updateLock)
            return;
        m_updateLock = true;
        ui->direction->setValue(m_mapController->overrideDirection());
        ui->timestamp->setDateTime(QDateTime::currentDateTime());
        m_updateLock = false;
        updatePosition();
    });

    mapView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    mapView->setSource(QUrl(QStringLiteral("qrc:/gammaray/positioning/mapview.qml")));

    connect(ui->actionCenterOn, &QAction::triggered, m_mapController, &MapController::centerOnPosition);
    addAction(ui->actionCenterOn);
    connect(ui->actionLoadNMEA, &QAction::triggered, this, &PositioningWidget::loadNmeaFile);
    addAction(ui->actionLoadNMEA);

    new PropertyBinder(m_interface, "positioningOverrideAvailable", ui->overrideBox, "enabled");
    new PropertyBinder(m_interface, "positioningOverrideEnabled", ui->overrideBox, "checked");
    new PropertyBinder(m_interface, "positioningOverrideEnabled", m_mapController, "overrideEnabled");

    updateWidgetState();
}

PositioningWidget::~PositioningWidget() = default;

void PositioningWidget::updatePosition()
{
    if (m_updateLock || !ui->overrideBox->isChecked())
        return;
    m_updateLock = true;

    QGeoPositionInfo info;
    info.setCoordinate(QGeoCoordinate(ui->latitude->value(), ui->longitude->value(), ui->altitude->value()));
    info.setTimestamp(ui->timestamp->dateTime());
    info.setAttribute(QGeoPositionInfo::Direction, ui->direction->value());
    info.setAttribute(QGeoPositionInfo::GroundSpeed, ui->horizontalSpeed->value());
    info.setAttribute(QGeoPositionInfo::VerticalSpeed, ui->verticalSpeed->value());
    info.setAttribute(QGeoPositionInfo::MagneticVariation, ui->magneticVariation->value());
    info.setAttribute(QGeoPositionInfo::HorizontalAccuracy, ui->horizontalAccuracy->value());
    info.setAttribute(QGeoPositionInfo::VerticalAccuracy, ui->verticalAccuracy->value());
    m_interface->setPositionInfoOverride(info);

    m_mapController->setOverrideCoordinate(QGeoCoordinate(ui->latitude->value(), ui->longitude->value()));
    m_mapController->setOverrideHorizontalAccuracy(ui->horizontalAccuracy->value());
    m_mapController->setOverrideDirection(ui->direction->value());

    m_updateLock = false;
}

void PositioningWidget::replayPosition()
{
    const auto pos = m_replaySource->lastKnownPosition();
    setUiToPosition(pos);
    updatePosition();
}

void PositioningWidget::loadNmeaFile()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Load NMEA file"));
    if (fileName.isEmpty())
        return;

    QScopedPointer<QFile> file(new QFile(fileName, this));
    if (!file->open(QFile::ReadOnly)) {
        QMessageBox::critical(this, tr("Failed to open NMEA file"), tr("Could not open '%1': %2.").arg(fileName, file->errorString()));
        return;
    }

    if (m_replaySource) {
        auto dev = m_replaySource->device();
        delete m_replaySource;
        delete dev;
    }

    m_replaySource = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, this);
    m_replaySource->setDevice(file.take());
    connect(m_replaySource, &QGeoPositionInfoSource::positionUpdated, this, &PositioningWidget::replayPosition);
    m_replaySource->startUpdates();

    // TODO error handling
    connect(m_replaySource, &QNmeaPositionInfoSource::updateTimeout, this, []() {
        qDebug() << "NMEA source update timeout!";
    });
    connect(m_replaySource, QOverload<QGeoPositionInfoSource::Error>::of(&QGeoPositionInfoSource::error), this, &PositioningWidget::nmeaError);
}

void PositioningWidget::nmeaError()
{
    qDebug() << m_replaySource->error();
}

void PositioningWidget::updateWidgetState()
{
    const auto e = ui->overrideBox->isEnabled() && ui->overrideBox->isChecked();
    ui->latitude->setEnabled(e);
    ui->longitude->setEnabled(e);
    ui->horizontalSpeed->setEnabled(e);
    ui->horizontalAccuracy->setEnabled(e);
    ui->altitude->setEnabled(e);
    ui->verticalSpeed->setEnabled(e);
    ui->verticalAccuracy->setEnabled(e);
    ui->direction->setEnabled(e);
    ui->magneticVariation->setEnabled(e);
    ui->timestamp->setEnabled(e);

    ui->actionLoadNMEA->setEnabled(e);
}

void PositioningWidget::setUiToPosition(const QGeoPositionInfo &pos)
{
    m_updateLock = true;
    if (pos.coordinate().type() != QGeoCoordinate::InvalidCoordinate) {
        ui->latitude->setValue(pos.coordinate().latitude());
        ui->longitude->setValue(pos.coordinate().longitude());
    }
    if (pos.coordinate().type() == QGeoCoordinate::Coordinate3D)
        ui->altitude->setValue(pos.coordinate().altitude());

    if (pos.hasAttribute(QGeoPositionInfo::Direction))
        ui->direction->setValue(pos.attribute(QGeoPositionInfo::Direction));
    if (pos.hasAttribute(QGeoPositionInfo::GroundSpeed))
        ui->horizontalSpeed->setValue(pos.attribute(QGeoPositionInfo::GroundSpeed));
    if (pos.hasAttribute(QGeoPositionInfo::VerticalSpeed))
        ui->verticalSpeed->setValue(pos.attribute(QGeoPositionInfo::VerticalSpeed));
    if (pos.hasAttribute(QGeoPositionInfo::MagneticVariation))
        ui->magneticVariation->setValue(pos.attribute(QGeoPositionInfo::MagneticVariation));
    if (pos.hasAttribute(QGeoPositionInfo::HorizontalAccuracy))
        ui->horizontalAccuracy->setValue(pos.attribute(QGeoPositionInfo::HorizontalAccuracy));
    if (pos.hasAttribute(QGeoPositionInfo::VerticalAccuracy))
        ui->verticalAccuracy->setValue(pos.attribute(QGeoPositionInfo::VerticalAccuracy));
    ui->timestamp->setDateTime(pos.timestamp());
    m_updateLock = false;
}

void PositioningUiFactory::initUi()
{
    qRegisterMetaTypeStreamOperators<QGeoPositionInfo>("QGeoPositionInfo");
}
