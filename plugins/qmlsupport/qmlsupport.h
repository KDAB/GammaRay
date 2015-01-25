/*
  qmlsupport.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_QMLSUPPORT_QMLSUPPORT_H
#define GAMMARAY_QMLSUPPORT_QMLSUPPORT_H

#include <core/toolfactory.h>

#include <QQmlEngine>

namespace GammaRay {

class QmlSupport : public QObject
{
  Q_OBJECT
public:
  explicit QmlSupport(ProbeInterface *probe, QObject *parent = 0);
};

class QmlSupportFactory : public QObject, public StandardToolFactory<QQmlEngine, QmlSupport>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.QmlSupport")

public:
  explicit QmlSupportFactory(QObject *parent = 0) : QObject(parent)
  {
  }

  QString name() const Q_DECL_OVERRIDE;
};

}

#endif
