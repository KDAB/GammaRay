/*
  resourcebrowserclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef RESOURCEBROWSERCLIENT_H
#define RESOURCEBROWSERCLIENT_H

#include <common/tools/resourcebrowser/resourcebrowserinterface.h>

namespace GammaRay {
class ResourceBrowserClient : public ResourceBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ResourceBrowserInterface)
public:
    explicit ResourceBrowserClient(QObject *parent);
    ~ResourceBrowserClient() override;

    void downloadResource(const QString &sourceFilePath,
                          const QString &targetFilePath) override;
    void selectResource(const QString &sourceFilePath, int line = -1,
                        int column = -1) override;
};
}

#endif // RESOURCEBROWSERCLIENT_H
