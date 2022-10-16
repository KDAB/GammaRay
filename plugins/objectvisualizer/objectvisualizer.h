/*
  objectvisualizer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZER_H
#define GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZER_H

#include <core/toolfactory.h>

namespace GammaRay {
class GraphViewer : public QObject
{
    Q_OBJECT

public:
    explicit GraphViewer(Probe *probe, QObject *parent = nullptr);
    ~GraphViewer() override;
};

class GraphViewerFactory : public QObject, public StandardToolFactory<QObject, GraphViewer>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_objectvisualizer.json")

public:
    explicit GraphViewerFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_GRAPHVIEWER_H
