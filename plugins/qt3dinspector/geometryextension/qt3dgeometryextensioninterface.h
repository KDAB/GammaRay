/*
  qt3dgeometryextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
#define GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {

class Qt3DGeometryExtensionInterface : public QObject
{
    Q_OBJECT
public:
    explicit Qt3DGeometryExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~Qt3DGeometryExtensionInterface();
};
}

Q_DECLARE_INTERFACE(GammaRay::Qt3DGeometryExtensionInterface, "com.kdab.GammaRay.Qt3DGeometryExtensionInterface/1.0")

#endif // GAMMARAY_QT3DGEOMETRYEXTENSIONINTERFACE_H
