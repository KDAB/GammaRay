/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "localeinspector.h"

#include "localemodel.h"
#include "localeaccessormodel.h"
#include "localedataaccessor.h"
#include "timezonemodel.h"
#include "timezonemodelroles.h"
#include "timezoneoffsetdatamodel.h"

#include <core/remote/serverproxymodel.h>
#include <common/objectbroker.h>

#include <QDebug>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

using namespace GammaRay;

LocaleInspector::LocaleInspector(Probe *probe, QObject *parent)
    : QObject(parent)
{
    auto *registry = new LocaleDataAccessorRegistry(this);

    auto *model = new LocaleModel(registry, this);
    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(model);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LocaleModel"), proxy);

    auto *accessorModel = new LocaleAccessorModel(registry, this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LocaleAccessorModel"), accessorModel);

    auto tzModel = new TimezoneModel(this);
    proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(tzModel);
    proxy->addRole(TimezoneModelRoles::LocalZoneRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TimezoneModel"), proxy);

    auto tzSelModel = ObjectBroker::selectionModel(proxy);
    connect(tzSelModel, &QItemSelectionModel::selectionChanged, this, &LocaleInspector::timezoneSelected);

    m_offsetModel = new TimezoneOffsetDataModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TimezoneOffsetDataModel"), m_offsetModel);
}

void LocaleInspector::timezoneSelected(const QItemSelection& selection)
{
    if (selection.isEmpty())
        return;
    auto idx = selection.first().topLeft();
    idx = idx.sibling(idx.row(), 0);
    m_offsetModel->setTimezone(QTimeZone(idx.data().toString().toUtf8()));
}
