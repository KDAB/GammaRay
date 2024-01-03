/*
  resourcefiltermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_RESOURCEBROWSER_RESOURCEFILTERMODEL_H
#define GAMMARAY_RESOURCEBROWSER_RESOURCEFILTERMODEL_H

#include <QSortFilterProxyModel>

namespace GammaRay {
class ResourceFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ResourceFilterModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
}

#endif // RESOURCEFILTERMODEL_H
