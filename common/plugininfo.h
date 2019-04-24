/*
  plugininfo.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PLUGININFO_H
#define GAMMARAY_PLUGININFO_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <qplugin.h>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace GammaRay {
/** Meta-data about a specific plugin.
 *  This abstracts Qt5-style embedded JSON data and Qt4-style desktop files.
 */
class PluginInfo
{
public:
    PluginInfo();
    explicit PluginInfo(const QString &path);
    explicit PluginInfo(const QStaticPlugin &staticPlugin);

    QString path() const;
    QString id() const;
    QString interfaceId() const;
    QStringList supportedTypes() const;
    QString name() const;
    bool remoteSupport() const;
    bool isHidden() const;
    QVector<QByteArray> selectableTypes() const;

    bool isValid() const;
    bool isStatic() const;

    QObject* staticInstance() const;

private:
    void init();
    void initFromJSON(const QString &path);
    void initFromJSON(const QJsonObject& metaData);

    QString m_path;
    QStaticPlugin m_staticPlugin;
    QString m_id;
    QString m_interface;
    QStringList m_supportedTypes;
    QString m_name;
    QVector<QByteArray> m_selectableTypes;
    bool m_remoteSupport;
    bool m_hidden;
};
}

#endif // GAMMARAY_PLUGININFO_H
