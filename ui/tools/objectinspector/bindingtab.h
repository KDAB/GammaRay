/*
  bindingtab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_BINDINGTAB_H
#define GAMMARAY_BINDINGTAB_H

// Qt
#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
class PropertyWidget;

namespace Ui {
class BindingTab;
}

class BindingTab : public QWidget
{
    Q_OBJECT
public:
    explicit BindingTab(PropertyWidget *parent = nullptr);
    ~BindingTab() override;

private slots:
    void bindingContextMenu(const QPoint &pos);

private:
    QScopedPointer<Ui::BindingTab> ui;
};
}

#endif // GAMMARAY_BINDINGTAB_H
