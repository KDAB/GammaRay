/*
  codecbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CODECBROWSERWIDGET_CODECBROWSERWIDGET_H
#define GAMMARAY_CODECBROWSERWIDGET_CODECBROWSERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class CodecBrowserWidget;
}

class CodecBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CodecBrowserWidget(QWidget *parent = nullptr);
    ~CodecBrowserWidget() override;

private slots:
    static void textChanged(const QString &text);

private:
    QScopedPointer<Ui::CodecBrowserWidget> ui;
    UIStateManager m_stateManager;
};

class CodecBrowserUiFactory : public QObject, public StandardToolUiFactory<CodecBrowserWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_codecbrowser.json")
};
}

#endif // GAMMARAY_CODECBROWSERWIDGET_H
