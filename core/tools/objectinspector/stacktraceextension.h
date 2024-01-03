/*
  stacktraceextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STACKTRACEEXTENSION_H
#define GAMMARAY_STACKTRACEEXTENSION_H

#include <core/propertycontrollerextension.h>

namespace GammaRay {
class StackTraceModel;

class StackTraceExtension : public PropertyControllerExtension
{
public:
    explicit StackTraceExtension(PropertyController *controller);
    ~StackTraceExtension();

    bool setQObject(QObject *object) override;

private:
    StackTraceModel *m_model;
};
}

#endif // GAMMARAY_STACKTRACEEXTENSION_H
