/*
  mimetypes.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "mimetypes.h"
#include "mimetypesmodel.h"

#include <3rdparty/kde/krecursivefilterproxymodel.h>

using namespace GammaRay;

MimeTypes::MimeTypes(Probe *probe, QObject *parent)
    : QObject(parent)
{
    auto model = new MimeTypesModel(this);
    auto proxy = new KRecursiveFilterProxyModel(this);
    proxy->setSourceModel(model);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MimeTypeModel"), proxy);
}

MimeTypes::~MimeTypes() = default;
