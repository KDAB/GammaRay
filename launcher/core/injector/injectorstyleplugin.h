/*
  injectorstyleplugin.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_INJECTORSTYLEPLUGIN_H
#define GAMMARAY_INJECTORSTYLEPLUGIN_H

#include <QStylePlugin>

namespace GammaRay {
class InjectorStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "injectorstyle.json")
public:
    QStyle *create(const QString &) override;
    static QStringList keys();

private:
    static void inject();
};
}

#endif // GAMMARAY_INJECTORSTYLEPLUGIN_H
