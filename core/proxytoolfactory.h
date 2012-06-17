/*
  proxytoolfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_PROXYTOOLFACTORY_H
#define GAMMARAY_PROXYTOOLFACTORY_H

#include "include/toolfactory.h"

namespace GammaRay {

/**
 * A wrapper around a plugin ToolFactory that only loads the actual plugin
 * once initialized.
 * Until then, meta-data is provided based on a plugin spec file.
 *
 * TODO: Improve error reporting
 */
class ProxyToolFactory : public QObject, public ToolFactory
{
  Q_OBJECT
  public:
    /**
     * @param path Path to the plugin spec file
     */
    explicit ProxyToolFactory(const QString &path, QObject *parent = 0);

    /** Returns @c true if the plugin seems valid from all the information we have so far. */
    bool isValid() const;

    /** Returns a human-readable error string if loading failed */
    QString errorString() const { return m_errorString; }

    virtual QString id() const;
    virtual QString name() const;
    virtual QStringList supportedTypes() const;

    virtual void init(ProbeInterface *probe);
    virtual QWidget *createWidget(ProbeInterface *probe, QWidget *parentWidget);

  private:
    QString m_id;
    QString m_pluginPath;
    QString m_name;
    QString m_errorString;
    QStringList m_supportedTypes;
    ToolFactory *m_factory;
};

}

#endif // GAMMARAY_PROXYTOOLFACTORY_H
