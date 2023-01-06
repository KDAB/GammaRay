/*
  abstractstyleelementstatetable.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTSTATETABLE_H
#define GAMMARAY_STYLEINSPECTOR_ABSTRACTSTYLEELEMENTSTATETABLE_H

#include "abstractstyleelementmodel.h"
#include <common/modelroles.h>

QT_BEGIN_NAMESPACE
class QStyleOption;
class QRect;
class QPainter;
QT_END_NAMESPACE

namespace GammaRay {
class StyleInspectorInterface;

/**
 * Base class for style element x style option state tables.
 * Covers the state part, sub-classes need to fill in the corresponding rows.
 */
class AbstractStyleElementStateTable : public GammaRay::AbstractStyleElementModel
{
    Q_OBJECT
public:
    explicit AbstractStyleElementStateTable(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

protected:
    int doColumnCount() const override;
    QVariant doData(int row, int column, int role) const override;

    /// standard setup for the style option used in a cell in column @p column
    void fillStyleOption(QStyleOption *option, int column) const;

protected:
    StyleInspectorInterface *m_interface;

private slots:
    void cellSizeChanged();
};
}

#endif // GAMMARAY_ABSTRACTSTYLEELEMENTSTATETABLE_H
