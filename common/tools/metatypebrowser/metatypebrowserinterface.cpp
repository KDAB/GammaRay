/*
  metatypebrowserinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "metatypebrowserinterface.h"

#include <common/objectbroker.h>

using namespace GammaRay;

MetaTypeBrowserInterface::MetaTypeBrowserInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<MetaTypeBrowserInterface *>(this);
}

MetaTypeBrowserInterface::~MetaTypeBrowserInterface() = default;
