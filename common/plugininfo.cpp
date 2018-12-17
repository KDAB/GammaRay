/*
  plugininfo.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "plugininfo.h"
#include "paths.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QLocale>
#include <QSettings>
#include <QCoreApplication>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QPluginLoader>

using namespace GammaRay;

PluginInfo::PluginInfo()
{
  init();
}

PluginInfo::PluginInfo(const QString& path)
{
    init();
    // OSX has broken QLibrary::isLibrary() - QTBUG-50446
    if (QLibrary::isLibrary(path) || path.endsWith(Paths::pluginExtension(), Qt::CaseInsensitive))
        initFromJSON(path);
    else if (path.endsWith(QLatin1String(".desktop")))
        initFromDesktopFile(path);
    else
        qDebug("%s: %s not a library, nor a .desktop file.", Q_FUNC_INFO, qPrintable(path));
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
PluginInfo::PluginInfo(const QStaticPlugin &staticPlugin)
{
    init();
    m_staticPlugin = staticPlugin;
    initFromJSON(staticPlugin.metaData());
}
#endif

void PluginInfo::init()
{
    m_remoteSupport = true;
    m_hidden = false;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    m_staticPlugin.instance = nullptr;
    m_staticPlugin.rawMetaData = nullptr;
#endif
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

    foreach (auto name, names) {
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
    return m_staticPlugin.instance && m_staticPlugin.rawMetaData;
}

QObject* PluginInfo::staticInstance() const
{
    Q_ASSERT(isStatic());
    return m_staticPlugin.instance();
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

    const auto selectable = customData.value(QStringLiteral("selectableTypes")).toArray();
    m_selectableTypes.reserve(selectable.size());
    for (auto it = selectable.begin(); it != selectable.end(); ++it)
        m_selectableTypes.push_back((*it).toString().toUtf8());
}

void PluginInfo::initFromDesktopFile(const QString &path)
{
    const QFileInfo fi(path);
    QSettings desktopFile(path, QSettings::IniFormat);
    desktopFile.beginGroup(QStringLiteral("Desktop Entry"));

    m_id = desktopFile.value(QStringLiteral("X-GammaRay-Id")).toString();
    m_interface
        = desktopFile.value(QStringLiteral("X-GammaRay-ServiceTypes"), QString()).toString();
    m_supportedTypes = desktopFile.value(QStringLiteral("X-GammaRay-Types")).toString().split(QLatin1Char(
                                                                                                  ';'),
                                                                                              QString::SkipEmptyParts);
    m_name = desktopFile.value(QStringLiteral("Name")).toString();
    m_remoteSupport = desktopFile.value(QStringLiteral("X-GammaRay-Remote"), true).toBool();
    m_hidden = desktopFile.value(QStringLiteral("Hidden"), false).toBool();

    const auto selectable
        = desktopFile.value(QStringLiteral("X-GammaRay-SelectableTypes")).toString().split(QLatin1Char(
                                                                                               ';'),
                                                                                           QString::SkipEmptyParts);
    m_selectableTypes.reserve(selectable.size());
    foreach (const auto &t, selectable)
        m_selectableTypes.push_back(t.toUtf8());

    const QString dllBaseName = desktopFile.value(QStringLiteral("Exec")).toString();
    if (dllBaseName.isEmpty())
        return;

    foreach (const QString &entry,
             fi.dir().entryList(QStringList(dllBaseName + QLatin1Char('*')), QDir::Files)) {
        const QString pluginPath = fi.dir().absoluteFilePath(entry);
        if (QLibrary::isLibrary(pluginPath)) {
            m_path = pluginPath;
            break;
        }
    }
}
