/*
  metatypebrowser.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "metatypebrowser.h"
#include "metatypesmodel.h"

#include <core/remote/serverproxymodel.h>

#include <common/objectbroker.h>
#include <common/tools/metatypebrowser/metatyperoles.h>

#include <QSortFilterProxyModel>

using namespace GammaRay;

MetaTypeBrowser::MetaTypeBrowser(Probe *probe, QObject *parent)
    : MetaTypeBrowserInterface(parent)
    , m_mtm(new MetaTypesModel(this))
{
    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(m_mtm);
    proxy->addRole(MetaTypeRoles::MetaObjectIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MetaTypeModel"), proxy);
}

void MetaTypeBrowser::rescanTypes()
{
    m_mtm->scanMetaTypes();
}
