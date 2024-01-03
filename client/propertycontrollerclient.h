/*
  propertycontrollerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYCONTROLLERCLIENT_H
#define GAMMARAY_PROPERTYCONTROLLERCLIENT_H

#include <common/propertycontrollerinterface.h>

namespace GammaRay {
class PropertyControllerClient : public PropertyControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PropertyControllerInterface)
public:
    explicit PropertyControllerClient(const QString &name, QObject *parent = nullptr);
    ~PropertyControllerClient() override;
};
}

#endif // GAMMARAY_PROPERTYCONTROLLERCLIENT_H
