/*
  positioningwidget.cpp

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

#include "positioningwidget.h"
#include "ui_positioningwidget.h"
#include "positioningclient.h"

#include <ui/propertybinder.h>
#include <common/objectbroker.h>

#include <QDateTime>
#include <QDebug>

using namespace GammaRay;

static QObject *createPositioningClient(const QString &name, QObject *parent)
{
    Q_UNUSED(name);
    return new PositioningClient(parent);
}

PositioningWidget::PositioningWidget(QWidget* parent):
    QWidget(parent),
    ui(new Ui::PositioningWidget)
{
    ui->setupUi(this);

    qRegisterMetaTypeStreamOperators<QGeoPositionInfo>("QGeoPositionInfo");
    ObjectBroker::registerClientObjectFactoryCallback<PositioningInterface*>(createPositioningClient);

    m_interface = ObjectBroker::object<PositioningInterface*>();
    Q_ASSERT(m_interface);

    new PropertyBinder(m_interface, "positioningOverrideEnabled", ui->overrideBox, "checked");

    connect(ui->latitudeBox, SIGNAL(valueChanged(double)), this, SLOT(updatePosition()));
    connect(ui->longitudeBox, SIGNAL(valueChanged(double)), this, SLOT(updatePosition()));
}

PositioningWidget::~PositioningWidget()
{
}

void PositioningWidget::updatePosition()
{
    QGeoPositionInfo info;
    info.setCoordinate(QGeoCoordinate(ui->latitudeBox->value(), ui->longitudeBox->value()));
    info.setTimestamp(QDateTime::currentDateTime());
    m_interface->setPositionInfoOverride(info);
}
