/*
  resourcebrowser.cpp

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

#include "resourcebrowser.h"
#include "resourcefiltermodel.h"

#include "qt/resourcemodel.h"
#include "common/objectbroker.h"

#include <core/remote/serverproxymodel.h>

#include <QDebug>
#include <QItemSelectionModel>
#include <QPixmap>

using namespace GammaRay;

ResourceBrowser::ResourceBrowser(ProbeInterface *probe, QObject *parent)
  : ResourceBrowserInterface(parent)
{
  ResourceModel *resourceModel = new ResourceModel(this);
  auto proxy = new ServerProxyModel<ResourceFilterModel>(this);
  proxy->setSourceModel(resourceModel);
  probe->registerModel(QStringLiteral("com.kdab.GammaRay.ResourceModel"), proxy);
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
  connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          this, SLOT(currentChanged(QModelIndex)));
}

void ResourceBrowser::downloadResource(const QString &sourceFilePath, const QString &targetFilePath)
{
  const QFileInfo fi(sourceFilePath);

  if (fi.isFile()) {
    static const QStringList l = QStringList() << QStringLiteral("jpg") << QStringLiteral("png") << QStringLiteral("jpeg");
    if (l.contains(fi.suffix())) {
      emit resourceDownloaded(targetFilePath, QPixmap(fi.absoluteFilePath()));
    } else {
      QFile f(fi.absoluteFilePath());
      if (f.open(QFile::ReadOnly | QFile::Text)) {
        emit resourceDownloaded(targetFilePath, f.readAll());
      } else {
        qWarning() << "Failed to open" << fi.absoluteFilePath();
      }
    }
  }
}

void ResourceBrowser::currentChanged(const QModelIndex &current)
{
  const QFileInfo fi(current.data(ResourceModel::FilePathRole).toString());

  if (fi.isFile()) {
    static const QStringList l = QStringList() << QStringLiteral("jpg") << QStringLiteral("png") << QStringLiteral("jpeg");
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

QString ResourceBrowserFactory::name() const
{
  return tr("Resources");
}
