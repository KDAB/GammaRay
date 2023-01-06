/*
  resourcefiltermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_RESOURCEBROWSER_RESOURCEFILTERMODEL_H
#define GAMMARAY_RESOURCEBROWSER_RESOURCEFILTERMODEL_H

#include <3rdparty/kde/krecursivefilterproxymodel.h>

namespace GammaRay {
class ResourceFilterModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT
public:
    explicit ResourceFilterModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};
}

#endif // RESOURCEFILTERMODEL_H
