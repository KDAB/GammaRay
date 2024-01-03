/*
  materialextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSIONCLIENT_H
#define GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSIONCLIENT_H

#include "materialextensioninterface.h"

namespace GammaRay {
class MaterialExtensionClient : public MaterialExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MaterialExtensionInterface)

public:
    explicit MaterialExtensionClient(const QString &name, QObject *parent = nullptr);
    ~MaterialExtensionClient() override;

public slots:
    void getShader(int row) override;
};
}

#endif // GAMMARAY_MATERIALEXTENSIONCLIENT_H
