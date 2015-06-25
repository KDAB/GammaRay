/*
  methodsextensionclient.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_METHODSEXTENSIONCLIENT_H
#define GAMMARAY_METHODSEXTENSIONCLIENT_H

#include <common/tools/objectinspector/methodsextensioninterface.h>

namespace GammaRay {

class MethodsExtensionClient : public MethodsExtensionInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::MethodsExtensionInterface)
  public:
    explicit MethodsExtensionClient(const QString &name, QObject *parent = 0);
    ~MethodsExtensionClient();

  public slots:
    void activateMethod() Q_DECL_OVERRIDE;
    void invokeMethod(Qt::ConnectionType type) Q_DECL_OVERRIDE;
    void connectToSignal() Q_DECL_OVERRIDE;
};

}

#endif // GAMMARAY_METHODSEXTENSIONCLIENT_H
