/*
  resourcebrowser.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
        | QItemSelectionModel::Rows
        | QItemSelectionModel::Current;
    const Qt::MatchFlags matchFlags = Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap;
    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ResourceModel"));
    auto selectionModel = ObjectBroker::selectionModel(model);
    const QString filePath = QLatin1Char(':') + QUrl(sourceFilePath).path();
    const QModelIndex index = model->match(model->index(0,
                                                        0),
                                           ResourceModel::FilePathRole, filePath, 1,
                                           matchFlags)
                                  .value(0);
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
