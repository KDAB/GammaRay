/*
  resourcebrowser.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_RESOURCEBROWSER_H
#define GAMMARAY_RESOURCEBROWSER_H

#include <qwidget.h>
#include <toolfactory.h>

class QItemSelection;

namespace GammaRay {

namespace Ui { class ResourceBrowser; }

class ResourceBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit ResourceBrowser(ProbeInterface *probe, QWidget *parent = 0);

  private slots:
    void resourceSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void setupLayout();

  private:
    QScopedPointer<Ui::ResourceBrowser> ui;
};

class ResourceBrowserFactory : public QObject, public StandardToolFactory<QObject, ResourceBrowser>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit ResourceBrowserFactory(QObject *parent) : QObject(parent) {}
    inline QString name() const { return tr("Resources"); }
};

}

#endif // GAMMARAY_RESOURCEBROWSER_H
