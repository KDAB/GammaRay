/*
  uiextractor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (mp.isValid() && mp.isDesignable(obj) && mp.isStored(obj) && mp.isWritable()) {
#else
    if (mp.isValid() && mp.isDesignable() && mp.isStored() && mp.isWritable()) {
#endif
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
