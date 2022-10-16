/*
  promolabel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROMOLABEL_H
#define GAMMARAY_PROMOLABEL_H

#include <ui/themedimagelabel.h>

namespace GammaRay {
class PromoLabel : public ThemedImageLabel
{
    Q_OBJECT

public:
    explicit PromoLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    bool event(QEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

private:
    void updatePixmap() override;
};
}

#endif // PROMOLABEL_H
