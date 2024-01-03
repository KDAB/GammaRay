/*
  fontbrowserinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "fontbrowserinterface.h"

#include <common/objectbroker.h>

using namespace GammaRay;

FontBrowserInterface::FontBrowserInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<FontBrowserInterface *>(this);
}

FontBrowserInterface::~FontBrowserInterface() = default;
