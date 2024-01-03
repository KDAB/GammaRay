/*
  modelinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MODELINSPECTORCLIENT_H
#define GAMMARAY_MODELINSPECTORCLIENT_H

#include "modelinspectorinterface.h"

namespace GammaRay {
class ModelInspectorClient : public ModelInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ModelInspectorInterface)
public:
    explicit ModelInspectorClient(QObject *parent = nullptr);
    ~ModelInspectorClient() override;
};
}

#endif // GAMMARAY_MODELINSPECTORCLIENT_H
