/*
  metatypebrowser.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METATYPEBROWSER_METATYPEBROWSER_H
#define GAMMARAY_METATYPEBROWSER_METATYPEBROWSER_H

#include <core/toolfactory.h>

#include <common/tools/metatypebrowser/metatypebrowserinterface.h>

namespace GammaRay {

class MetaTypesModel;

class MetaTypeBrowser : public MetaTypeBrowserInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MetaTypeBrowserInterface)
public:
    explicit MetaTypeBrowser(Probe *probe, QObject *parent = nullptr);

public slots:
    void rescanTypes() override;

private:
    MetaTypesModel *m_mtm;
};

class MetaTypeBrowserFactory : public QObject, public StandardToolFactory<QObject, MetaTypeBrowser>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
public:
    explicit MetaTypeBrowserFactory(QObject *parent)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_METATYPEBROWSER_H
