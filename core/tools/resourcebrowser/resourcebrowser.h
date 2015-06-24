/*
  resourcebrowser.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSER_H
#define GAMMARAY_RESOURCEBROWSER_RESOURCEBROWSER_H

#include "toolfactory.h"
#include <common/resourcebrowserinterface.h>

class QModelIndex;

namespace GammaRay {

class ResourceBrowser : public ResourceBrowserInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ResourceBrowserInterface)
  public:
    explicit ResourceBrowser(ProbeInterface *probe, QObject *parent = 0);

  public slots:
    void downloadResource(const QString &sourceFilePath, const QString &targetFilePath) Q_DECL_OVERRIDE;

  private slots:
    void currentChanged(const QModelIndex &current);
};

class ResourceBrowserFactory : public QObject, public StandardToolFactory<QObject, ResourceBrowser>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ResourceBrowserFactory(QObject *parent) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("Resources");
    }
};

}

#endif // GAMMARAY_RESOURCEBROWSER_H
