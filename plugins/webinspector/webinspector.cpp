/*
  webinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "webinspector.h"
#include "webviewmodel.h"

#include <common/endpoint.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>

#include <QDebug>
#include <QUrl>
#include <QtPlugin>

using namespace GammaRay;

WebInspector::WebInspector(Probe *probe, QObject *parent)
    : QObject(parent)
{
    auto *webViewModel = new WebViewModel(this);
    webViewModel->setSourceModel(probe->objectListModel());
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.WebPages"), webViewModel);

    connect(probe, &Probe::objectCreated, this, &WebInspector::objectAdded);

    const QUrl serverUrl = Endpoint::instance()->serverAddress();
    QString serverAddress(GAMMARAY_DEFAULT_ANY_ADDRESS);
    if (serverUrl.scheme() == QLatin1String("tcp"))
        serverAddress = serverUrl.host();
    // see also probecreator.cpp
    qputenv("QTWEBKIT_INSPECTOR_SERVER",
            serverAddress.toLocal8Bit() + ':' + QByteArray::number(Endpoint::defaultPort() + 1));
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", QByteArray::number(Endpoint::defaultPort() + 1));
}

void WebInspector::objectAdded(QObject *obj)
{
    // both of the following cases seem to be needed, the web view object changes depending on
    // you have "import QtWebKit.experimental 1.0" or not...
    QObject *experimental = nullptr;
    if (obj->inherits("QQuickWebView"))
        experimental = obj->property("experimental").value<QObject *>();
    if (obj->inherits("QQuickWebViewExperimental"))
        experimental = obj;

    if (!experimental)
        return;

    // FIXME: this conversion fails with "QMetaProperty::read: Unable to handle unregistered datatype 'QWebPreferences*' for property 'QQuickWebViewExperimental::preferences'"
    // if we don't have "import QtWebKit.experimental 1.0"
    QObject *prefs = experimental->property("preferences").value<QObject *>();
    if (!prefs)
        return;
    prefs->setProperty("developerExtrasEnabled", true);
}

WebInspectorFactory::WebInspectorFactory(QObject *parent)
    : QObject(parent)
{
}

QString WebInspectorFactory::id() const
{
    return WebInspector::staticMetaObject.className();
}

void WebInspectorFactory::init(Probe *probe)
{
    new WebInspector(probe, probe);
}
