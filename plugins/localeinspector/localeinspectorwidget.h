/*
  localeinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEINSPECTORWIDGET_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEINSPECTORWIDGET_H

#include <ui/tooluifactory.h>

#include <QWidget>

#include <memory>

namespace GammaRay {
namespace Ui {
class LocaleInspectorWidget;
}

class LocaleInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LocaleInspectorWidget(QWidget *parent = nullptr);
    ~LocaleInspectorWidget() override;

private:
    std::unique_ptr<Ui::LocaleInspectorWidget> ui;
};

class LocaleInspectorUiFactory : public QObject, public StandardToolUiFactory<LocaleInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_localeinspector.json")
};

}

#endif // GAMMARAY_LOCALEINSPECTORWIDGET_H
