/*
  localetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOCALETAB_H
#define GAMMARAY_LOCALETAB_H

#include <ui/uistatemanager.h>

#include <QWidget>

#include <memory>

namespace GammaRay {

namespace Ui {
class LocaleTab;
}

class LocaleTab : public QWidget
{
    Q_OBJECT
public:
    explicit LocaleTab(QWidget *parent = nullptr);
    ~LocaleTab() override;

private slots:
    void initSplitterPosition();

private:
    std::unique_ptr<Ui::LocaleTab> ui;
    UIStateManager m_stateManager;
};

}

#endif // GAMMARAY_LOCALETAB_H
