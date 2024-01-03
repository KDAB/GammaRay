/*
  styleinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORWIDGET_H
#define GAMMARAY_STYLEINSPECTOR_STYLEINSPECTORWIDGET_H

#include <ui/tooluifactory.h>
#include <QWidget>

namespace GammaRay {
namespace Ui {
class StyleInspectorWidget;
}

class StyleInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StyleInspectorWidget(QWidget *parent = nullptr);
    ~StyleInspectorWidget() override;

private slots:
    void styleSelected(int index);

private:
    Ui::StyleInspectorWidget *ui;
};

class StyleInspectorUiFactory : public QObject, public StandardToolUiFactory<StyleInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_styleinspector.json")
};
}

#endif // GAMMARAY_STYLEINSPECTORWIDGET_H
