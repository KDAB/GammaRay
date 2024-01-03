/*
  stacktraceextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "stacktraceextension.h"
#include "stacktracemodel.h"

#include <core/probe.h>
#include <core/propertycontroller.h>

#include <QDebug>

using namespace GammaRay;

StackTraceExtension::StackTraceExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".stackTrace")
    , m_model(new StackTraceModel(controller))
{
    controller->registerModel(m_model, QStringLiteral("stackTraceModel"));
}

StackTraceExtension::~StackTraceExtension() = default;

bool StackTraceExtension::setQObject(QObject *object)
{
    const auto trace = Probe::instance()->objectCreationStackTrace(object);
    m_model->setStackTrace(trace);
    return !trace.empty();
}
