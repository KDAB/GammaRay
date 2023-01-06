/*
  toolmanagerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TOOLMANAGERCLIENT_H
#define GAMMARAY_TOOLMANAGERCLIENT_H

#include "gammaray_ui_export.h"

#include <common/toolmanagerinterface.h>

namespace GammaRay {
class ToolManagerClient : public ToolManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolManagerInterface)
public:
    explicit ToolManagerClient(QObject *parent = nullptr);

    void selectObject(const ObjectId &id, const QString &toolId) override;
    void requestToolsForObject(const ObjectId &id) override;
    void requestAvailableTools() override;
};
}

#endif // GAMMARAY_TOOLMANAGERCLIENT_H
