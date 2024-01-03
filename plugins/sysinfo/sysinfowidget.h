/*
  sysinfowidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SYSINFOWIDGET_H
#define GAMMARAY_SYSINFOWIDGET_H

#include <ui/tooluifactory.h>

#include <QWidget>

#include <memory>

namespace GammaRay {

namespace Ui {
class SysInfoWidget;
}

class SysInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SysInfoWidget(QWidget *parent = nullptr);
    ~SysInfoWidget() override;

private:
    std::unique_ptr<Ui::SysInfoWidget> ui;
};

class SysInfoUiFactory : public QObject, public StandardToolUiFactory<SysInfoWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_sysinfo.json")
};

}

#endif // GAMMARAY_SYSINFOWIDGET_H
