/*
  resourcebrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "resourcebrowser.h"
#include "resourcefiltermodel.h"

#include "qt/resourcemodel.h"
#include "common/objectbroker.h"

#include <QDebug>
#include <QItemSelectionModel>
#include <QPixmap>

using namespace GammaRay;

ResourceBrowser::ResourceBrowser(ProbeInterface *probe, QObject *parent)
  : ResourceBrowserInterface(parent)
{
  ResourceModel *resourceModel = new ResourceModel(this);
  ResourceFilterModel *proxy = new ResourceFilterModel(this);
  proxy->setSourceModel(resourceModel);
  probe->registerModel("com.kdab.GammaRay.ResourceModel", proxy);
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
  connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          this, SLOT(currentChanged(QModelIndex)));
}

void ResourceBrowser::currentChanged(const QModelIndex &current)
{
  const QFileInfo fi(current.data(ResourceModel::FilePathRole).toString());

  if (fi.isFile()) {
    static const QStringList l = QStringList() << "jpg" << "png" << "jpeg";
    if (l.contains(fi.suffix())) {
      emit resourceSelected(QPixmap(fi.absoluteFilePath()));
    } else {
      QFile f(fi.absoluteFilePath());
      if (f.open(QFile::ReadOnly | QFile::Text)) {
        emit resourceSelected(f.readAll());
      } else {
        qWarning() << "Failed to open" << fi.absoluteFilePath();
        emit resourceDeselected();
      }
    }
  } else {
    emit resourceDeselected();
  }
}

