/*
  objectvisualizerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZERWIDGET_H
#define GAMMARAY_OBJECTVISUALIZER_OBJECTVISUALIZERWIDGET_H

#include <ui/tooluifactory.h>
#include <ui/uistatemanager.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class DeferredTreeView;
class GraphWidget;

class GraphViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GraphViewerWidget(QWidget *parent = nullptr);
    virtual ~GraphViewerWidget();

private:
    UIStateManager m_stateManager;
    QAbstractItemModel *mModel;
    DeferredTreeView *mObjectTreeView;
    GraphWidget *mWidget;
};

class ObjectVisualizerUiFactory : public QObject, public StandardToolUiFactory<GraphViewerWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_objectvisualizer.json")
};
}

#endif // GAMMARAY_GRAPHVIEWER_H
