/*
  resourcebrowser.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSER_H
#define GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSER_H

#include "toolfactory.h"
#include <common/tools/resourcebrowser/resourcebrowserinterface.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class ResourceBrowser : public ResourceBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ResourceBrowserInterface)
public:
    explicit ResourceBrowser(Probe *probe, QObject *parent = nullptr);

public slots:
    void downloadResource(const QString &sourceFilePath,
                          const QString &targetFilePath) override;
    void selectResource(const QString &sourceFilePath, int line = -1,
                        int column = -1) override;

private slots:
    void currentChanged(const QModelIndex &current, int line = -1, int column = -1);
};

class ResourceBrowserFactory : public QObject, public StandardToolFactory<QObject, ResourceBrowser>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
public:
    explicit ResourceBrowserFactory(QObject *parent)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_RESOURCEBROWSER_H
