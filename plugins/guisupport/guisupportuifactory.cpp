/*
  guisupportuifactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "guisupportuifactory.h"
#include "paintanalyzertab.h"

#include <ui/propertywidget.h>

using namespace GammaRay;

QString GuiSupportUiFactory::id() const
{
    return QString();
}

void GuiSupportUiFactory::initUi()
{
    PropertyWidget::registerTab<PaintAnalyzerTab>(QStringLiteral("painting"), tr("Paint Analyzer"),
                                                  PropertyWidgetTabPriority::Advanced);
}

QWidget *GuiSupportUiFactory::createWidget(QWidget *)
{
    return nullptr;
}
