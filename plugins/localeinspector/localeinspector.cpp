/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include "timezonemodel.h"
#endif

#include <core/remote/serverproxymodel.h>

#include <QSortFilterProxyModel>

using namespace GammaRay;

LocaleInspector::LocaleInspector(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    LocaleDataAccessorRegistry *registry = new LocaleDataAccessorRegistry(this);

    LocaleModel *model = new LocaleModel(registry, this);
    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(model);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LocaleModel"), proxy);

    LocaleAccessorModel *accessorModel = new LocaleAccessorModel(registry, this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.LocaleAccessorModel"), accessorModel);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    auto tzModel = new TimezoneModel(this);
    proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(tzModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TimezoneModel"), proxy);
#endif
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(LocaleInspectorFactory)
#endif
