/*
  messagehandlerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_MESSAGEHANDLERCLIENT_H
#define GAMMARAY_MESSAGEHANDLERCLIENT_H

#include <common/tools/messagehandler/messagehandlerinterface.h>

namespace GammaRay {
class MessageHandlerClient : public MessageHandlerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MessageHandlerInterface)
public:
    explicit MessageHandlerClient(QObject *parent = nullptr);

public slots:
    void generateFullTrace() override;
};
}

#endif // GAMMARAY_MESSAGEHANDLERCLIENT_H
