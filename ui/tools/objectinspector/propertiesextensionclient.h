/*
  propertiesextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTIESEXTENSIONCLIENT_H
#define GAMMARAY_PROPERTIESEXTENSIONCLIENT_H

#include <common/tools/objectinspector/propertiesextensioninterface.h>

namespace GammaRay {
class PropertiesExtensionClient : public PropertiesExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PropertiesExtensionInterface)
public:
    explicit PropertiesExtensionClient(const QString &name, QObject *parent = nullptr);
    ~PropertiesExtensionClient() override;

public slots:
    void setProperty(const QString &propertyName, const QVariant &value) override;
};
}

#endif // GAMMARAY_PROPERTIESEXTENSIONCLIENT_H
