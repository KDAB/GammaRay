/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTOR_H

#include <core/toolfactory.h>
#include "actioninspectorwidget.h"

#include <QAction>

namespace GammaRay {

class ActionInspector : public QObject
{
  Q_OBJECT

  public:
    explicit ActionInspector(ProbeInterface *probe, QObject *parent = 0);
    virtual ~ActionInspector();

  public Q_SLOTS:
    void triggerAction(int row);
};

class ActionInspectorFactory : public QObject,
    public StandardToolFactory2<QAction, ActionInspector, ActionInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.ActionInspector")

  public:
    explicit ActionInspectorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual inline QString name() const
    {
      return tr("Action Inspector");
    }
};

}

#endif // GAMMARAY_ACTIONINSPECTOR_H
