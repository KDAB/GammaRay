/*
  pixelmetricmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_PIXELMETRICMODEL_H
#define GAMMARAY_STYLEINSPECTOR_PIXELMETRICMODEL_H

#include "abstractstyleelementmodel.h"

namespace GammaRay {
/**
 * Lists all pixel metric values of a given QStyle.
 */
class PixelMetricModel : public AbstractStyleElementModel
{
    Q_OBJECT
public:
    explicit PixelMetricModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

protected:
    QVariant doData(int row, int column, int role) const override;
    int doColumnCount() const override;
    int doRowCount() const override;
};
}

#endif // GAMMARAY_PIXELMETRICMODEL_H
