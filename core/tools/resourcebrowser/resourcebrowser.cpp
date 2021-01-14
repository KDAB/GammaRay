/*
  resourcebrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QUrl>

using namespace GammaRay;

ResourceBrowser::ResourceBrowser(Probe *probe, QObject *parent)
    : ResourceBrowserInterface(parent)
{
    auto *resourceModel = new ResourceModel(this);
    auto proxy = new ServerProxyModel<ResourceFilterModel>(this);
    proxy->setSourceModel(resourceModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ResourceModel"), proxy);
    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
    connect(selectionModel, &QItemSelectionModel::currentChanged,
            this, [this](const QModelIndex &index) { currentChanged(index); });
}

void ResourceBrowser::downloadResource(const QString &sourceFilePath, const QString &targetFilePath)
{
    const QFileInfo fi(sourceFilePath);

    if (fi.isFile()) {
        QFile f(fi.absoluteFilePath());
        if (f.open(QFile::ReadOnly))
            emit resourceDownloaded(targetFilePath, f.readAll());
        else
            qWarning() << "Failed to open" << fi.absoluteFilePath();
    }
}

void ResourceBrowser::selectResource(const QString &sourceFilePath, int line, int column)
{
    const bool locked = blockSignals(true);
    const QItemSelectionModel::SelectionFlags selectionFlags = QItemSelectionModel::ClearAndSelect
                                                               |QItemSelectionModel::Rows
                                                               | QItemSelectionModel::Current;
    const Qt::MatchFlags matchFlags = Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap;
    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ResourceModel"));
    auto selectionModel = ObjectBroker::selectionModel(model);
    const QString filePath = QLatin1Char(':') + QUrl(sourceFilePath).path();
    const QModelIndex index = model->match(model->index(0,
                                                        0), ResourceModel::FilePathRole, filePath, 1,
                                           matchFlags).value(0);
    selectionModel->setCurrentIndex(index, selectionFlags);
    blockSignals(locked);
    currentChanged(index, line, column);
}

void ResourceBrowser::currentChanged(const QModelIndex &current, int line, int column)
{
    if (!current.isValid())
        return;
    const auto idx = current.sibling(current.row(), 0);
    const QFileInfo fi(idx.data(ResourceModel::FilePathRole).toString());
    if (!fi.isFile()) {
        emit resourceDeselected();
        return;
    }

    QFile f(fi.absoluteFilePath());
    if (f.open(QFile::ReadOnly)) {
        emit resourceSelected(f.readAll(), line, column);
    } else {
        qWarning() << "Failed to open" << fi.absoluteFilePath();
        emit resourceDeselected();
    }
}
