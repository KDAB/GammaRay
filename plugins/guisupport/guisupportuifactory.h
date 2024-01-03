/*
  guisupportuifactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_GUISUPPORTUIFACTORY_H
#define GAMMARAY_GUISUPPORTUIFACTORY_H

#include <ui/tooluifactory.h>

namespace GammaRay {
class GuiSupportUiFactory : public QObject, public ToolUiFactory
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_guisupport.json")
public:
    QString id() const override;
    void initUi() override;
    QWidget *createWidget(QWidget *) override;
};
}

#endif // GAMMARAY_GUISUPPORTUIFACTORY_H
