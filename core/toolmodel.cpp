/*
  toolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "toolmodel.h"

#include "toolfactory.h"
#include "proxytoolfactory.h"
#include "probe.h"
#include "probesettings.h"

#include "tools/localeinspector/localeinspector.h"
#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/modelinspector/modelinspector.h"
#include "tools/objectinspector/objectinspector.h"
#include "tools/resourcebrowser/resourcebrowser.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/messagehandler/messagehandler.h"
#include "tools/metaobjectbrowser/metaobjectbrowser.h"
#include "metaobjectrepository.h"
#include "metaobject.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include "tools/standardpaths/standardpaths.h"
#include "tools/mimetypes/mimetypes.h"
#endif

#include <common/pluginmanager.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibrary>
#include <QMetaMethod>
#include <QThread>

using namespace GammaRay;

ToolModel::ToolModel(QObject *parent): QAbstractListModel(parent)
{
  // built-in tools
  addToolFactory(new ObjectInspectorFactory(this));
  addToolFactory(new ModelInspectorFactory(this));
  addToolFactory(new ResourceBrowserFactory(this));
  addToolFactory(new MetaObjectBrowserFactory(this));
  addToolFactory(new MetaTypeBrowserFactory(this));
  addToolFactory(new TextDocumentInspectorFactory(this));
  addToolFactory(new MessageHandlerFactory(this));
  addToolFactory(new LocaleInspectorFactory(this));
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  addToolFactory(new StandardPathsFactory(this));
  addToolFactory(new MimeTypesFactory(this));
#endif

  m_pluginManager.reset(new ToolPluginManager(this));
  Q_FOREACH (ToolFactory *factory, m_pluginManager->plugins()) {
    addToolFactory(factory);
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
  } else if (role == ToolModelRole::ToolEnabled) {
    return !m_inactiveTools.contains(toolIface);
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
  map.insert(ToolModelRole::ToolEnabled, data(index, ToolModelRole::ToolEnabled));
  // the other custom roles are useless on the client anyway, since they contain raw pointers
  return map;
}

void ToolModel::objectAdded(QObject *obj)
{
  Q_ASSERT(QThread::currentThread() == thread());
  Q_ASSERT(Probe::instance()->isValidObject(obj));

  // m_knownMetaObjects allows us to skip the expensive recursive search for matching tools
  if (!m_knownMetaObjects.contains(obj->metaObject())) {
    objectAdded(obj->metaObject());
    m_knownMetaObjects.insert(obj->metaObject());
  }
}

void ToolModel::objectAdded(const QMetaObject *mo)
{
  Q_ASSERT(thread() == QThread::currentThread());
  foreach (ToolFactory *factory, m_inactiveTools) {
    if (factory->supportedTypes().contains(mo->className())) {
      m_inactiveTools.remove(factory);
      factory->init(Probe::instance());
      const int row = m_tools.indexOf(factory);
      emit dataChanged(index(row, 0), index(row, 0));
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

QModelIndex ToolModel::toolForObject(QObject* object) const
{
  if (!object)
    return QModelIndex();
  const QMetaObject *metaObject = object->metaObject();
  while (metaObject) {
    for (int i = 0; i < m_tools.size(); i++) {
      const ToolFactory *factory = m_tools.at(i);
      if (factory && factory->supportedTypes().contains(metaObject->className())) {
        return index(i, 0);
      }
    }
    metaObject = metaObject->superClass();
  }
  return QModelIndex();
}

QModelIndex ToolModel::toolForObject(const void* object, const QString& typeName) const
{
  if (!object)
    return QModelIndex();
  const MetaObject *metaObject = MetaObjectRepository::instance()->metaObject(typeName);
  while (metaObject) {
    for (int i = 0; i < m_tools.size(); i++) {
      const ToolFactory *factory = m_tools.at(i);
      if (factory && factory->supportedTypes().contains(metaObject->className())) {
        return index(i, 0);
      }
    }
    metaObject = metaObject->superClass();
  }
  return QModelIndex();
}

void ToolModel::addToolFactory(ToolFactory* tool)
{
  if (!tool->isHidden())
    m_tools.push_back(tool);
  m_inactiveTools.insert(tool);
}
