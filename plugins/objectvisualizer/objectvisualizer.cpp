/*
  objectvisualizer.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectvisualizer.h"
#include "objectvisualizermodel.h"

#include <core/remote/serverproxymodel.h>

#include <QtPlugin>

using namespace GammaRay;

GraphViewer::GraphViewer(Probe *probe, QObject *parent)
    : QObject(parent)
{
    auto model = new ServerProxyModel<ObjectVisualizerModel>(this);
    model->setSourceModel(probe->objectTreeModel());
    model->addProxyRole(ObjectVisualizerModel::ObjectId);
    model->addProxyRole(ObjectVisualizerModel::ObjectDisplayName);
    model->addProxyRole(ObjectVisualizerModel::ClassName);
    probe->registerModel("com.kdab.GammaRay.ObjectVisualizerModel", model);
}

GraphViewer::~GraphViewer() = default;
