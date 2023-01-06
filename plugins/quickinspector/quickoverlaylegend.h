/*
  quickoverlaylegend.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKOVERLAYLEGEND_H
#define GAMMARAY_QUICKINSPECTOR_QUICKOVERLAYLEGEND_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace GammaRay {
struct QuickDecorationsSettings;
class LegendModel;

class QuickOverlayLegend : public QWidget
{
    Q_OBJECT
public:
    explicit QuickOverlayLegend(QWidget *parent = nullptr);

    QAction *visibilityAction() const;
    void setOverlaySettings(const QuickDecorationsSettings &settings);

    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    LegendModel *m_model;
    QAction *m_visibilityAction;
};

}

#endif // GAMMARAY_QUICKINSPECTOR_QUICKOVERLAYLEGEND_H
