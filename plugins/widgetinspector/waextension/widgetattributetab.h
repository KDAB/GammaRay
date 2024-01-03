/*
  widgetattributetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETATTRIBUTETAB_H
#define GAMMARAY_WIDGETATTRIBUTETAB_H

#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
class PropertyWidget;

namespace Ui {
class WidgetAttributeTab;
}

class WidgetAttributeTab : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetAttributeTab(PropertyWidget *parent = nullptr);
    ~WidgetAttributeTab() override;

private:
    QScopedPointer<Ui::WidgetAttributeTab> ui;
};
}

#endif // GAMMARAY_WIDGETATTRIBUTETAB_H
