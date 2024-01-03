/*
  networkconfigurationwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKCONFIGURATIONWIDGET_H
#define GAMMARAY_NETWORKCONFIGURATIONWIDGET_H

#include <QWidget>

#include <memory>

namespace GammaRay {

namespace Ui {
class NetworkConfigurationWidget;
}

class NetworkConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkConfigurationWidget(QWidget *parent = nullptr);
    ~NetworkConfigurationWidget() override;

private:
    std::unique_ptr<Ui::NetworkConfigurationWidget> ui;
};
}

#endif // GAMMARAY_NETWORKCONFIGURATIONWIDGET_H
