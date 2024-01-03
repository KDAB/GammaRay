/*
  cookieextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "cookieextension.h"
#include "cookiejarmodel.h"

#include <core/propertycontroller.h>

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>

using namespace GammaRay;

CookieExtension::CookieExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".cookieJar")
    , m_cookieJarModel(new CookieJarModel(controller))
{
    controller->registerModel(m_cookieJarModel, QStringLiteral("cookieJarModel"));
}

CookieExtension::~CookieExtension() = default;

bool CookieExtension::setQObject(QObject *object)
{
    if (auto cookieJar = qobject_cast<QNetworkCookieJar *>(object)) {
        m_cookieJarModel->setCookieJar(cookieJar);
        return true;
    } else if (auto nam = qobject_cast<QNetworkAccessManager *>(object)) {
        return setQObject(nam->cookieJar());
    }

    m_cookieJarModel->setCookieJar(nullptr);
    return false;
}
