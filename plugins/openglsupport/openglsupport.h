/*
  openglsupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_OPENGLSUPPORT_H
#define GAMMARAY_OPENGLSUPPORT_H

#include <core/toolfactory.h>

#include <QIcon>
#include <QSet>

QT_BEGIN_NAMESPACE
class QWindow;
QT_END_NAMESPACE

namespace GammaRay {
class OpenGLSupport : public QObject
{
    Q_OBJECT
public:
    explicit OpenGLSupport(Probe *probe, QObject *parent = nullptr);

private:
    static void registerMetaTypes();
    static void registerVariantHandler();
};

class OpenGLSupportFactory : public QObject, public StandardToolFactory<QObject, OpenGLSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_openglsupport.json")
public:
    explicit OpenGLSupportFactory(QObject *parent = nullptr);
};
}

#endif // GAMMARAY_OPENGLSUPPORT_H
