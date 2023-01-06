/*
  networkwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
