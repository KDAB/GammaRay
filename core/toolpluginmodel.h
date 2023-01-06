/*
  toolpluginmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TOOLPLUGINMODEL_H
#define GAMMARAY_TOOLPLUGINMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {
class ToolFactory;

/** Information about loaded plugins, for display in the about dialog. */
class ToolPluginModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ToolPluginModel(const QVector<ToolFactory *> &plugins, QObject *parent = nullptr);
    ~ToolPluginModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    QVector<ToolFactory *> m_tools;
};
}

#endif // GAMMARAY_TOOLPLUGINMODEL_H
