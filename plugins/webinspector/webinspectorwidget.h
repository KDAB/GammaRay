/*
  webinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WEBINSPECTOR_WEBINSPECTORWIDGET_H
#define GAMMARAY_WEBINSPECTOR_WEBINSPECTORWIDGET_H

#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class WebInspectorWidget;
}

class WebInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WebInspectorWidget(QWidget *parent = nullptr);
    ~WebInspectorWidget() override;

private slots:
    void webPageSelected(int index);

private:
    QScopedPointer<Ui::WebInspectorWidget> ui;
};

class WebInspectorUiFactory : public QObject, public StandardToolUiFactory<WebInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_webinspector.json")
};
}

#endif
