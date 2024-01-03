/*
  networkwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKWIDGET_H
#define GAMMARAY_NETWORKWIDGET_H

#include <ui/tooluifactory.h>

#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
namespace Ui {
class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget() override;

private:
    QScopedPointer<Ui::NetworkWidget> ui;
};

class NetworkWidgetFactory : public QObject, public StandardToolUiFactory<NetworkWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_network.json")
public:
    void initUi() override;
};
}

#endif // GAMMARAY_NETWORKWIDGET_H
