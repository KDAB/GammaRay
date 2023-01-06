/*
  toolpluginerrormodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TOOLPLUGINERRORMODEL_H
#define GAMMARAY_TOOLPLUGINERRORMODEL_H

#include <common/pluginmanager.h>

#include <QAbstractTableModel>

namespace GammaRay {
/** List of plugin loading errors on the target. */
class ToolPluginErrorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ToolPluginErrorModel(const PluginLoadErrors &errors, QObject *parent = nullptr);
    ~ToolPluginErrorModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    PluginLoadErrors m_errors;
};
}

#endif // GAMMARAY_TOOLPLUGINERRORMODEL_H
