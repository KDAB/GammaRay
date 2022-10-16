/*
  stacktracetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STACKTRACETAB_H
#define GAMMARAY_STACKTRACETAB_H

#include <QWidget>

#include <memory>

namespace GammaRay {
class PropertyWidget;
namespace Ui {
class StackTraceTab;
}

class StackTraceTab : public QWidget
{
    Q_OBJECT
public:
    explicit StackTraceTab(PropertyWidget *parent = nullptr);
    ~StackTraceTab() override;

private Q_SLOTS:
    void contextMenuRequested(QPoint pos);

private:
    std::unique_ptr<Ui::StackTraceTab> ui;
};
}

#endif // GAMMARAY_STACKTRACETAB_H
