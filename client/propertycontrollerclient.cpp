/*
  propertycontrollerclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertycontrollerclient.h"

#include <common/endpoint.h>
#include <common/metatypedeclarations.h>
#include <common/variantwrapper.h>
#include <common/streamoperators.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QTypeRevision>
#endif

using namespace GammaRay;

Q_DECLARE_METATYPE(QMarginsF)

static void registerMetaTypes()
{
    StreamOperators::registerOperators<QMargins>();
    StreamOperators::registerOperators<QMarginsF>();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // TODO: Probably should be somewhere else
    qRegisterMetaType<QTypeRevision>();
#endif
}

PropertyControllerClient::PropertyControllerClient(const QString &name, QObject *parent)
    : PropertyControllerInterface(name, parent)
{
    registerMetaTypes();
}

PropertyControllerClient::~PropertyControllerClient() = default;
