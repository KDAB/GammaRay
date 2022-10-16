/*
  localeinspector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

void LocaleInspector::timezoneSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    auto idx = selection.first().topLeft();
    idx = idx.sibling(idx.row(), 0);
    m_offsetModel->setTimezone(QTimeZone(idx.data().toString().toUtf8()));
}
