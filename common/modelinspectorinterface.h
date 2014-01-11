/*
  modelinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_MODELINSPECTORINTERFACE_H
#define GAMMARAY_MODELINSPECTORINTERFACE_H

#include <QObject>

namespace GammaRay {

class ModelInspectorInterface : public QObject
{
  Q_OBJECT
  public:
    explicit ModelInspectorInterface(QObject *parent = 0);
    virtual ~ModelInspectorInterface();

  signals:
    void cellSelected(int row, int col, const QString &internalId, const QString &internalPtr);
};

}

Q_DECLARE_INTERFACE(GammaRay::ModelInspectorInterface, "com.kdab.GammaRay.ModelInspectorInterface")

#endif // GAMMARAY_MODELINSPECTORINTERFACE_H
