/*
  standardpathswidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STANDARDPATHSWIDGET_H
#define GAMMARAY_STANDARDPATHSWIDGET_H

#include <ui/uistatemanager.h>

#include <QWidget>

#include <memory>

namespace GammaRay {
namespace Ui {
class StandardPathsWidget;
}

class StandardPathsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StandardPathsWidget(QWidget *parent = nullptr);
    ~StandardPathsWidget() override;

private:
    std::unique_ptr<Ui::StandardPathsWidget> ui;
    UIStateManager m_stateManager;
};
}

#endif // GAMMARAY_STANDARDPATHSWIDGET_H
