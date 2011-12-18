/*
  toolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "config-gammaray.h"
#include "toolmodel.h"
#include "toolfactory.h"

#include "tools/codecbrowser/codecbrowser.h"
#include "tools/connectioninspector/connectioninspector.h"
#include "tools/fontbrowser/fontbrowser.h"
#include "tools/localeinspector/localeinspector.h"
#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/modelinspector/modelinspector.h"
#include "tools/objectinspector/objectinspector.h"
#include "tools/resourcebrowser/resourcebrowser.h"
#include "tools/sceneinspector/sceneinspector.h"
#include "tools/selectionmodelinspector/selectionmodelinspector.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/widgetinspector/widgetinspector.h"
#include "tools/messagehandler/messagehandler.h"
#ifdef BUILD_TIMER_PLUGIN
#include "tools/timertop/timertop.h"
#endif

#include "pluginmanager.h"
#include "probe.h"
#include "readorwritelocker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QThread>

using namespace GammaRay;

ToolModel::ToolModel(QObject *parent): QAbstractListModel(parent)
{
  // built-in tools
  m_tools.push_back(new ObjectInspectorFactory(this));
  m_tools.push_back(new WidgetInspectorFactory(this));
  m_tools.push_back(new ModelInspector(this));
  m_tools.push_back(new SceneInspectorFactory(this));
  m_tools.push_back(new ConnectionInspectorFactory(this));
  m_tools.push_back(new ResourceBrowserFactory(this));
  m_tools.push_back(new MetaTypeBrowserFactory(this));
  m_tools.push_back(new SelectionModelInspectorFactory(this));
  m_tools.push_back(new FontBrowserFactory(this));
  m_tools.push_back(new CodecBrowserFactory(this));
  m_tools.push_back(new TextDocumentInspectorFactory(this));
  m_tools.push_back(new MessageHandlerFactory(this));
  m_tools.push_back(new LocaleInspectorFactory(this));
#ifdef BUILD_TIMER_PLUGIN
  m_tools.push_back(new TimerTopFactory(this));
#endif

  Q_FOREACH (ToolFactory *factory, PluginManager::instance()->plugins()) {
    m_tools.push_back(factory);
  }

  // everything but the object inspector is inactive initially
  for (int i = 1; i < m_tools.size(); ++i) {
    m_inactiveTools.insert(m_tools.at(i));
  }
}

QVariant ToolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Probe");
    default:
      return tr("N/A");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant ToolModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  ToolFactory *toolIface = m_tools.at(index.row());
  if (role == Qt::DisplayRole) {
    return toolIface->name();
  } else if (role == ToolFactoryRole) {
    return QVariant::fromValue(toolIface);
  } else if (role == ToolWidgetRole) {
    return QVariant::fromValue(m_toolWidgets.value(toolIface));
  } else if (role == ToolIdRole) {
    return toolIface->id();
  }
  return QVariant();
}

bool ToolModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && role == Qt::EditRole) {
    ToolFactory *toolIface = m_tools.at(index.row());
    m_toolWidgets.insert(toolIface, value.value<QWidget*>());
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

int ToolModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_tools.size();
}

Qt::ItemFlags ToolModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if (index.isValid()) {
    ToolFactory *toolIface = m_tools.at(index.row());
    if (m_inactiveTools.contains(toolIface)) {
      flags &= ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
  }
  return flags;
}

void ToolModel::objectAdded(QObject *obj)
{
  // delay to main thread if required
  QMetaObject::invokeMethod(this, "objectAddedMainThread",
                            Qt::AutoConnection, Q_ARG(QObject *, obj));
}

void ToolModel::objectAddedMainThread(QObject *obj)
{
  ReadOrWriteLocker lock(Probe::instance()->objectLock());

  if (Probe::instance()->isValidObject(obj)) {
    objectAdded(obj->metaObject());
  }
}

void ToolModel::objectAdded(const QMetaObject *mo)
{
  Q_ASSERT(thread() == QThread::currentThread());
  foreach (ToolFactory *factory, m_inactiveTools) {
    if (factory->supportedTypes().contains(mo->className())) {
      m_inactiveTools.remove(factory);
      factory->init(Probe::instance());
      emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
  }
  if (mo->superClass()) {
    objectAdded(mo->superClass());
  }
}

#include "toolmodel.moc"
