/*
  cookieextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_COOKIEEXTENSION_H
#define GAMMARAY_COOKIEEXTENSION_H

#include <core/propertycontrollerextension.h>

namespace GammaRay {
class CookieJarModel;

class CookieExtension : public PropertyControllerExtension
{
public:
    explicit CookieExtension(PropertyController *controller);
    ~CookieExtension();

    bool setQObject(QObject *object) override;

private:
    CookieJarModel *m_cookieJarModel;
};
}

#endif // GAMMARAY_COOKIEEXTENSION_H
