/*
  connectioninspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CONNECTIONINSPECTOR_CONNECTIONINSPECTOR_H
#define GAMMARAY_CONNECTIONINSPECTOR_CONNECTIONINSPECTOR_H

#include "include/toolfactory.h"

#include <QWidget>

namespace GammaRay {

namespace Ui {
  class ConnectionInspector;
}

class ConnectionInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit ConnectionInspector(ProbeInterface *probe, QWidget *parent = 0);

  private:
    QScopedPointer<Ui::ConnectionInspector> ui;
};

class ConnectionInspectorFactory : public QObject,
                                   public StandardToolFactory<QObject, ConnectionInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ConnectionInspectorFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Connections");
    }
};

}

#endif // GAMMARAY_CONNECTIONINSPECTOR_H
