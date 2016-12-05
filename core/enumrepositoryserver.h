/*
  enumrepositoryserver.h

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

#ifndef GAMMARAY_ENUMREPOSITORYSERVER_H
#define GAMMARAY_ENUMREPOSITORYSERVER_H

#include "gammaray_core_export.h"

#include <common/enumrepository.h>

#include <QHash>

QT_BEGIN_NAMESPACE
class QMetaEnum;
QT_END_NAMESPACE

namespace GammaRay {

/*! Probe-side enum definition management.
 * Use this to obtain enum value representations that can be safely transferred to the client.
 */
class EnumRepositoryServer : public EnumRepository
{
    Q_OBJECT
public:
    ~EnumRepositoryServer();

    /*! Creates an EnumValue instance for the given numeric value and QMetaEnum.
     *  If not yet present, the enum definition with be registered
     *  with the EnumRepositoryServer, so that the client has access
     *  to the key/value mappings of the enum if needed.
     */
    static GAMMARAY_CORE_EXPORT EnumValue valueFromMetaEnum(int value, const QMetaEnum &me);

    //! @cond internal
    static void create(QObject *parent);
    //! @endcond
private:
    explicit EnumRepositoryServer(QObject *parent = nullptr);
    void requestDefinition(EnumId id) Q_DECL_OVERRIDE;

    EnumId m_nextId;

    static EnumRepositoryServer *s_instance;

    QHash<QByteArray, EnumId> m_nameToIdMap;
};
}

#endif // GAMMARAY_ENUMREPOSITORYSERVER_H
