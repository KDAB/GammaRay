/*
  webinspector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    QByteArray addr = serverAddress.toLocal8Bit().append(':').append(QByteArray::number(Endpoint::defaultPort() + 1));
    qputenv("QTWEBKIT_INSPECTOR_SERVER", addr);
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
