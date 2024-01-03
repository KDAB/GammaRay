/*
  cookietab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_COOKIETAB_H
#define GAMMARAY_COOKIETAB_H

#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
class PropertyWidget;

namespace Ui {
class CookieTab;
}

class CookieTab : public QWidget
{
    Q_OBJECT
public:
    explicit CookieTab(PropertyWidget *parent = nullptr);
    ~CookieTab() override;

private:
    QScopedPointer<Ui::CookieTab> ui;
};
}

#endif // GAMMARAY_COOKIETAB_H
