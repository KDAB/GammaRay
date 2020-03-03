/*
  enumrepository.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ENUMREPOSITORY_H
#define GAMMARAY_ENUMREPOSITORY_H

#include "gammaray_common_export.h"

#include "enumdefinition.h"

#include <QObject>

namespace GammaRay {

/*! Collection of EnumDefinition and on-demand client/server transfer of those. */
class GAMMARAY_COMMON_EXPORT EnumRepository : public QObject
{
    Q_OBJECT
public:
    ~EnumRepository() override;

    /*! Request the enum definition for the given enum id.
     *  On the client side the response can be invalid on first request,
     *  as transfer from the server is asynchronous. Listen to the
     *  definitionChanged() signal to be notified for its availability
     *  in that case.
     */
    virtual EnumDefinition definition(EnumId id) const;

signals:
    /*! Notification of EnumDefinition availability.
     *  @see definition()
     */
    void definitionChanged(int id);

    //! @cond internal
    void definitionResponse(const GammaRay::EnumDefinition &def);

protected:
    EnumRepository(QObject *parent = nullptr);
    void addDefinition(const EnumDefinition &def);

    Q_INVOKABLE virtual void requestDefinition(int id) = 0;
    //! @endcond

private:
    QVector<EnumDefinition> m_definitions;
};

}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::EnumRepository, "com.kdab.GammaRay.EnumRepository/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_ENUMREPOSITORY_H
