/*
  plugininfo.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

    QObject *staticInstance() const;

private:
    void initFromJSON(const QString &path);
    void initFromJSON(const QJsonObject &metaData);

    QString m_path;
    QtPluginInstanceFunction m_staticInstanceFunc = nullptr;
    QString m_id;
    QString m_interface;
    QStringList m_supportedTypes;
    QString m_name;
    QVector<QByteArray> m_selectableTypes;
    bool m_remoteSupport = true;
    bool m_hidden = false;
};
}

#endif // GAMMARAY_PLUGININFO_H
