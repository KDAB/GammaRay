/*
  plugininfo.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "plugininfo.h"
#include "paths.h"

#include <compat/qasconst.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QLocale>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

using namespace GammaRay;

PluginInfo::PluginInfo() = default;

PluginInfo::PluginInfo(const QString &path)
{
    // OSX has broken QLibrary::isLibrary() - QTBUG-50446
    if (QLibrary::isLibrary(path) || path.endsWith(Paths::pluginExtension(), Qt::CaseInsensitive))
        initFromJSON(path);
}

PluginInfo::PluginInfo(const QStaticPlugin &staticPlugin)
{
    m_staticInstanceFunc = staticPlugin.instance;
    initFromJSON(staticPlugin.metaData());
}

QString PluginInfo::path() const
{
    return m_path;
}

QString PluginInfo::id() const
{
    return m_id;
}

QString PluginInfo::interfaceId() const
{
    return m_interface;
}

QStringList PluginInfo::supportedTypes() const
{
    return m_supportedTypes;
}

bool PluginInfo::remoteSupport() const
{
    return m_remoteSupport;
}

QString PluginInfo::name() const
{
    return m_name;
}

bool PluginInfo::isHidden() const
{
    return m_hidden;
}

QVector<QByteArray> PluginInfo::selectableTypes() const
{
    return m_selectableTypes;
}

bool PluginInfo::isValid() const
{
    return !m_id.isEmpty() && (isStatic() || !m_path.isEmpty()) && !m_interface.isEmpty();
}

static QString readLocalized(const QLocale &locale, const QJsonObject &obj, const QString &baseKey)
{
    const QString qtcLanguage = qApp->property("qtc_locale").toString();
    QStringList names = locale.uiLanguages();
    if (!qtcLanguage.isEmpty())
        names.prepend(qtcLanguage);

    for (auto name : qAsConst(names)) {
        const QLocale uiLocale(name);

        // We are natively English, skip...
        if (uiLocale.language() == QLocale::English || uiLocale.language() == QLocale::C) {
            return obj.value(baseKey).toString();
        }

        // Check against name
        QString key = baseKey + '[' + name + ']';
        auto it = obj.find(key);

        // Check against language
        if (it == obj.end()) {
            name.replace('-', '_');
            name = name.section(QLatin1Char('_'), 0, -2);
            if (!name.isEmpty()) {
                key = baseKey + '[' + name + ']';
                it = obj.find(key);
            }
        }

        if (it != obj.end())
            return it.value().toString();
    }

    return obj.value(baseKey).toString();
}

bool PluginInfo::isStatic() const
{
    return m_staticInstanceFunc;
}

QObject *PluginInfo::staticInstance() const
{
    Q_ASSERT(isStatic());
    return m_staticInstanceFunc();
}

void PluginInfo::initFromJSON(const QString &path)
{
    const QPluginLoader loader(path);
    const QJsonObject metaData = loader.metaData();
    initFromJSON(metaData);
    m_path = path;
}

void PluginInfo::initFromJSON(const QJsonObject &metaData)
{
    m_interface = metaData.value(QStringLiteral("IID")).toString();
    const QJsonObject customData = metaData.value(QStringLiteral("MetaData")).toObject();

    m_id = customData.value(QStringLiteral("id")).toString();
    m_name = readLocalized(QLocale(), customData, QStringLiteral("name"));
    m_remoteSupport = customData.value(QStringLiteral("remoteSupport")).toBool(true);
    m_hidden = customData.value(QStringLiteral("hidden")).toBool(false);

    const QJsonArray types = customData.value(QStringLiteral("types")).toArray();
    m_supportedTypes.reserve(types.size());
    for (auto it = types.constBegin(); it != types.constEnd(); ++it)
        m_supportedTypes.push_back((*it).toString());

    const auto selectableTypes = customData.value(QStringLiteral("selectableTypes")).toArray();
    m_selectableTypes.reserve(selectableTypes.size());
    for (auto &&selectable : selectableTypes)
        m_selectableTypes.push_back(selectable.toString().toUtf8());
}
