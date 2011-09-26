/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Author: Milian Wolff, KDAB (milian.wolff@kdab.com)
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "gammarayplugin.h"

#include "gammaraytool.h"
#include "gammarayruncontrolfactory.h"
#include "gammaraysettings.h"

#include <analyzerbase/analyzermanager.h>
#include <analyzerbase/analyzersettings.h>

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>

using namespace Analyzer;
using namespace GammaRay;

////////////////////////////////////////////////////////////////////////
//
// GammaRayPlugin
//
////////////////////////////////////////////////////////////////////////

GammaRayPlugin::GammaRayPlugin()
{
}

GammaRayPlugin::~GammaRayPlugin()
{
}

static AbstractAnalyzerSubConfig *globalGammaRayFactory()
{
  return new GammaRayGlobalSettings();
}

static AbstractAnalyzerSubConfig *projectGammaRayFactory()
{
  return new GammaRayProjectSettings();
}

bool GammaRayPlugin::initialize(const QStringList &/*arguments*/, QString */*errorString*/)
{
  AnalyzerGlobalSettings::instance()->registerSubConfigs(&globalGammaRayFactory,
                                                         &projectGammaRayFactory);

  addAutoReleasedObject(new GammaRayRunControlFactory());

  StartModes modes;
  // They are handled the same actually.
  //modes.append(StartMode(StartRemote));
  modes.append(StartMode(StartLocal));
  AnalyzerManager::addTool(new GammaRayTool(this), modes);

  return true;
}

void GammaRayPlugin::extensionsInitialized()
{
  // Retrieve objects from the plugin manager's object pool.
  // "In the extensionsInitialized method, a plugin can be sure that all
  //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag GammaRayPlugin::aboutToShutdown()
{
  // Save settings.
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)
  return SynchronousShutdown;
}

Q_EXPORT_PLUGIN(GammaRayPlugin)
