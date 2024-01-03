/*
  standardiconmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STANDARDICONMODEL_H
#define GAMMARAY_STYLEINSPECTOR_STANDARDICONMODEL_H

#include "abstractstyleelementmodel.h"

#include <QStyle>

namespace GammaRay {
/**
 * Lists all standard icons of a style.
 */
class StandardIconModel : public AbstractStyleElementModel
{
    Q_OBJECT
public:
    explicit StandardIconModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

protected:
    QVariant doData(int row, int column, int role) const override;
    int doColumnCount() const override;
    int doRowCount() const override;

private:
    QVariant dataForStandardIcon(QStyle::StandardPixmap stdPix, const QString &name, int column,
                                 int role) const;
};
}

#endif // GAMMARAY_STANDARDICONMODEL_H
