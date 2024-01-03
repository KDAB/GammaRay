/*
  metatypebrowserclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
