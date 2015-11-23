/*
  quickinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

class QTimer;
class QLabel;
class QImage;
class QItemSelection;

namespace GammaRay {
class QuickScenePreviewWidget;

namespace Ui {
  class QuickInspectorWidget;
}

class QuickInspectorWidget : public QWidget
{
  Q_OBJECT

  enum Action {
      NavigateToCode
  };

  public:
    explicit QuickInspectorWidget(QWidget *parent = 0);
    ~QuickInspectorWidget();

  private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

  private slots:
    void sceneChanged();
    void sceneRendered (const GammaRay::TransferImage &image, const GammaRay::QuickItemGeometry &itemGeometry);
    void itemSelectionChanged(const QItemSelection &selection);
    void requestRender();
    void setFeatures(GammaRay::QuickInspectorInterface::Features features);
    void setSplitterSizes();
    void itemModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void itemContextMenu(const QPoint &pos);

  private:
    QScopedPointer<Ui::QuickInspectorWidget> ui;
    QuickScenePreviewWidget *m_previewWidget;
    QuickInspectorInterface *m_interface;
    QTimer *m_renderTimer;
    bool m_sceneChangedSinceLastRequest;
    bool m_waitingForImage;
};

class QuickInspectorUiFactory : public QObject, public StandardToolUiFactory<QuickInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_quickinspector.json")

  void initUi() Q_DECL_OVERRIDE;
};

}

#endif
