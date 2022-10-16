/*
  metatypebrowserclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METATYPEBROWSERCLIENT_H
#define GAMMARAY_METATYPEBROWSERCLIENT_H

#include <common/tools/metatypebrowser/metatypebrowserinterface.h>

namespace GammaRay {

class MetaTypeBrowserClient : public MetaTypeBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MetaTypeBrowserInterface)
public:
    explicit MetaTypeBrowserClient(QObject *parent);
    ~MetaTypeBrowserClient() override;

    void rescanTypes() override;
};
}

#endif // GAMMARAY_METATYPEBROWSERCLIENT_H
