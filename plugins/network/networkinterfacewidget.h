/*
  networkinterfacewidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKINTERFACEWIDGET_H
#define GAMMARAY_NETWORKINTERFACEWIDGET_H

#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
namespace Ui {
class NetworkInterfaceWidget;
}

class NetworkInterfaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkInterfaceWidget(QWidget *parent = nullptr);
    ~NetworkInterfaceWidget() override;

private:
    QScopedPointer<Ui::NetworkInterfaceWidget> ui;
};
}

#endif // GAMMARAY_NETWORKINTERFACEWIDGET_H
