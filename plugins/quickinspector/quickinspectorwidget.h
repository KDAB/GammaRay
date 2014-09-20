/*
  quickinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORWIDGET_H

#include <ui/tooluifactory.h>
#include <ui/propertywidget.h>
#include <common/objectbroker.h>
#include "quickinspectorinterface.h"

#include <QWidget>
#include <QVariant>

class QQuickItem;
class QQuickView;
class QTimer;
class QLabel;
class QImage;
class QItemSelection;

namespace GammaRay {
class QuickSceneImageProvider;

namespace Ui {
  class QuickInspectorWidget;
}

class QuickInspectorWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit QuickInspectorWidget(QWidget *parent = 0);
    ~QuickInspectorWidget();

  private:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

  private slots:
    void sceneChanged();
    void sceneRendered(const QVariantMap &previewData);
    void itemSelectionChanged(const QItemSelection &selection);
    void requestRender();
    void setFeatures(GammaRay::QuickInspectorInterface::Features features);
    void setSplitterSizes();
    void itemModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

  private:
    QScopedPointer<Ui::QuickInspectorWidget> ui;
    QuickInspectorInterface *m_interface;
    QTimer *m_renderTimer;
    bool m_sceneChangedSinceLastRequest;
    bool m_waitingForImage;
    QuickSceneImageProvider *m_imageProvider;
    QQuickItem *m_rootItem;
    QQuickView *m_preview;
};

class QuickInspectorUiFactory : public QObject, public StandardToolUiFactory<QuickInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.QuickInspectorUi")

  void initUi();
};

}

#endif
