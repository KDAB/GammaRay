/*
  uiextractor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "uiextractor.h"

#ifdef HAVE_QT_DESIGNER
#include <QDebug>
#include <QLocale>
#include <QMetaObject>
#include <QMetaProperty>

using namespace GammaRay;

bool UiExtractor::checkProperty(QObject *obj, const QString &prop) const
{
    const QMetaObject *mo = obj->metaObject();
    const QMetaProperty mp = mo->property(mo->indexOfProperty(prop.toLatin1()));

    // TODO come up with some more aggressive filtering
    if (mp.isValid() && mp.isDesignable(obj) && mp.isStored(obj) && mp.isWritable()) {
        const QVariant value = mp.read(obj);

        // try to figure out the default by resetting to it
        if (mp.isResettable()) {
            mp.reset(obj);
            if (mp.read(obj) == value)
                return false;
            mp.write(obj, value);
            return true;
        }

        // some guessing for non-resettable properties
        if (value.isNull() || !value.isValid())
            return false;

        if (value.type() == QVariant::String)
            return !value.toString().isEmpty();
        else if (value.type() == QVariant::Locale)
            return value.toLocale() != QLocale::system();

        return true;
    }

    return false;
}

#endif
