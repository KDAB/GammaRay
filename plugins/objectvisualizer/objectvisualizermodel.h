/*
  objectvisualizermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZERMODEL_H
#define GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZERMODEL_H

#include <common/objectmodel.h>
#include <3rdparty/kde/krecursivefilterproxymodel.h>

namespace GammaRay {
/** Augment the regular object tree by some information needed for the visualization
 * on the client side.
 */
class ObjectVisualizerModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT
public:
    enum Role
    {
        ObjectId = ObjectModel::UserRole,
        ObjectDisplayName,
        ClassName
    };

    explicit ObjectVisualizerModel(QObject *parent);
    ~ObjectVisualizerModel() override;

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_OBJECTVISUALIZERMODEL_H
