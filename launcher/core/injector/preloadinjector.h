/*
  preloadinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PRELOADINJECTOR_H
#define GAMMARAY_PRELOADINJECTOR_H

#include "processinjector.h"
#include <qglobal.h>

namespace GammaRay {
class PreloadInjector : public ProcessInjector
{
    Q_OBJECT
public:
    PreloadInjector();
    QString name() const override;
    bool launch(const QStringList &programAndArgs, const QString &probeDll,
                const QString &probeFunc, const QProcessEnvironment &_env) override;
};
}

#endif // GAMMARAY_PRELOADINJECTOR_H
