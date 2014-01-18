/*
  webinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_WEBINSPECTOR_WEBINSPECTOR_H
#define GAMMARAY_WEBINSPECTOR_WEBINSPECTOR_H

#include <core/toolfactory.h>

namespace GammaRay {

class WebInspector : public QObject
{
  Q_OBJECT
  public:
    explicit WebInspector(GammaRay::ProbeInterface *probe, QObject *parent = 0);

  private slots:
    void objectAdded(QObject *obj);
};

class WebInspectorFactory : public QObject, public ToolFactory
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.WebInspector")

  public:
    explicit WebInspectorFactory(QObject *parent = 0);
    QString id() const;
    QString name() const;
    void init(ProbeInterface* probe);
    QStringList supportedTypes() const;
    bool isHidden() const;
};

}

#endif // GAMMARAY_WEBINSPECTOR_H
