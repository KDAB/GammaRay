/*
  uiextractor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    if (mp.isValid() && mp.isDesignable() && mp.isStored() && mp.isWritable()) {
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

        if (value.typeId() == QMetaType::QString)
            return !value.toString().isEmpty();
        else if (value.typeId() == QMetaType::QLocale)
            return value.toLocale() != QLocale::system();

        return true;
    }

    return false;
}

#endif
