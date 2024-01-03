/*
  enumrepositoryclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_ENUMREPOSITORYCLIENT_H
#define GAMMARAY_ENUMREPOSITORYCLIENT_H

#include <common/enumrepository.h>

namespace GammaRay {

class EnumRepositoryClient : public GammaRay::EnumRepository
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::EnumRepository)
public:
    explicit EnumRepositoryClient(QObject *parent = nullptr);
    ~EnumRepositoryClient() override;

    EnumDefinition definition(EnumId id) const override;

private slots:
    void definitionReceived(const GammaRay::EnumDefinition &def);

private:
    void requestDefinition(EnumId id) override;
};
}

#endif // GAMMARAY_ENUMREPOSITORYCLIENT_H
