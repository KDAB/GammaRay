/*
  qmlsupport.h

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

#ifndef GAMMARAY_QUICKINSPECTOR_H
#define GAMMARAY_QUICKINSPECTOR_H

#include "quickinspectorinterface.h"

#include <core/toolfactory.h>

#include <QQuickWindow>

class QItemSelection;
class QItemSelectionModel;

namespace GammaRay {

class PropertyController;
class QuickItemModel;

class QuickInspector : public QuickInspectorInterface
{
  Q_OBJECT
public:
  explicit QuickInspector(ProbeInterface *probe, QObject *parent = 0);
  ~QuickInspector();

protected:
  bool eventFilter(QObject *receiver, QEvent* event) Q_DECL_OVERRIDE;

private slots:
  void frameSwapped();
  void itemSelectionChanged(const QItemSelection &selection);
  void clientConnectedChanged(bool connected);

private:
  void selectWindow(QQuickWindow* window);
  void selectItem(QQuickItem *item);
  void registerMetaTypes();

  QQuickItem* recursiveChiltAt(QQuickItem* parent, const QPointF& pos) const;

  ProbeInterface* m_probe;
  QPointer<QQuickWindow> m_window;
  QuickItemModel *m_itemModel;
  QItemSelectionModel *m_itemSelectionModel;
  PropertyController *m_propertyController;
  bool m_clientConnected;
};

class QuickInspectorFactory : public QObject, public StandardToolFactory<QQuickWindow, QuickInspector>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.QuickInspector")

public:
  explicit QuickInspectorFactory(QObject *parent = 0) : QObject(parent)
  {
  }

  QString name() const Q_DECL_OVERRIDE;
};

}

#endif
