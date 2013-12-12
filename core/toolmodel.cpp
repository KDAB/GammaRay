/*
  toolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "proxytoolfactory.h"

#include "tools/connectioninspector/connectioninspector.h"
#include "tools/localeinspector/localeinspector.h"
#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/modelinspector/modelinspector.h"
#include "tools/objectinspector/objectinspector.h"
#include "tools/resourcebrowser/resourcebrowser.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/messagehandler/messagehandler.h"
#include "tools/metaobjectbrowser/metaobjectbrowser.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include "tools/standardpaths/standardpaths.h"
#include "tools/mimetypes/mimetypes.h"
#endif

#include <common/pluginmanager.h>
#include "probe.h"
#include "readorwritelocker.h"
#include "probesettings.h"

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
  m_tools.push_back(new ModelInspectorFactory(this));
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  m_tools.push_back(new ConnectionInspectorFactory(this));
#endif
  m_tools.push_back(new ResourceBrowserFactory(this));
  m_tools.push_back(new MetaObjectBrowserFactory(this));
  m_tools.push_back(new MetaTypeBrowserFactory(this));
  m_tools.push_back(new TextDocumentInspectorFactory(this));
  m_tools.push_back(new MessageHandlerFactory(this));
  m_tools.push_back(new LocaleInspectorFactory(this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  m_tools.push_back(new StandardPathsFactory(this));
  m_tools.push_back(new MimeTypesFactory(this));
#endif

  const QString pluginPath = ProbeSettings::value("ProbePath").toString() + QDir::separator() + QLatin1String(GAMMARAY_RELATIVE_PROBE_TO_PLUGIN_PATH);
  m_pluginManager.reset(new ToolPluginManager(pluginPath, this));
  Q_FOREACH (ToolFactory *factory, m_pluginManager->plugins()) {
    m_tools.push_back(factory);
  }

  // everything but the object inspector is inactive initially
  const int numberOfTools(m_tools.size());
  for (int i = 0; i < numberOfTools; ++i) {
    m_inactiveTools.insert(m_tools.at(i));
  }
}

ToolModel::~ToolModel()
{
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
  } else if (role == ToolModelRole::ToolFactory) {
    return QVariant::fromValue(toolIface);
  } else if (role == ToolModelRole::ToolId) {
    return toolIface->id();
  }
  return QVariant();
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

QMap<int, QVariant> ToolModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> map = QAbstractListModel::itemData(index);
  map.insert(ToolModelRole::ToolId, data(index, ToolModelRole::ToolId));
  // the other custom roles are useless on the client anyway, since they contain raw pointers
  return map;
}

void ToolModel::objectAdded(QObject *obj)
{
  // delay to main thread if required
  QMetaObject::invokeMethod(this, "objectAddedMainThread",
                            Qt::AutoConnection, Q_ARG(QObject*, obj));
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

QVector< ToolFactory* > ToolModel::plugins() const
{
  return m_pluginManager->plugins();
}

PluginLoadErrors ToolModel::pluginErrors() const
{
  return m_pluginManager->errors();
}

