/*
  enumrepositoryclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
