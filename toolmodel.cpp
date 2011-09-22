/*
  toolmodel.cpp

  This file is part of Gammaray, the Qt application inspection and
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

#include "toolmodel.h"
#include "toolfactory.h"

#include "tools/codecbrowser/codecbrowser.h"
#include "tools/connectioninspector/connectioninspector.h"
#include "tools/fontbrowser/fontbrowser.h"
#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/modelinspector/modelinspector.h"
#include "tools/objectinspector/objectinspector.h"
#include "tools/resourcebrowser/resourcebrowser.h"
#include "tools/sceneinspector/sceneinspector.h"
#include "tools/scriptenginedebugger/scriptenginedebugger.h"
#include "tools/selectionmodelinspector/selectionmodelinspector.h"
#include "tools/statemachineinspector/statemachineinspector.h"
#include "tools/textdocumentinspector/textdocumentinspector.h"
#include "tools/webinspector/webinspector.h"
#include "tools/widgetinspector/widgetinspector.h"

#include <QDebug>
#include "probe.h"

using namespace Gammaray;

ToolModel::ToolModel(QObject *parent): QAbstractListModel(parent)
{
  // built-in tools
  m_tools.push_back(new ObjectInspectorFactory(this));
  m_tools.push_back(new WidgetInspectorFactory(this));
  m_tools.push_back(new ModelInspector(this));
  m_tools.push_back(new SceneInspectorFactory(this));
  m_tools.push_back(new ScriptEngineDebuggerFactory(this));
  m_tools.push_back(new WebInspectorFactory(this));
  m_tools.push_back(new ConnectionInspectorFactory(this));
  m_tools.push_back(new ResourceBrowserFactory(this));
  m_tools.push_back(new StateMachineInspectorFactory(this));
  m_tools.push_back(new MetaTypeBrowserFactory(this));
  m_tools.push_back(new SelectionModelInspectorFactory(this));
  m_tools.push_back(new FontBrowserFactory(this));
  m_tools.push_back(new CodecBrowserFactory(this));
  m_tools.push_back(new TextDocumentInspectorFactory(this));

  // tool plugins
  // TODO

  // everything but the object inspector is inactive initially
  for (int i = 1; i < m_tools.size(); ++i) {
    m_inactiveTools.insert(m_tools.at(i));
  }
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

void ToolModel::objectAdded(const QPointer< QObject > &obj)
{
  if (obj) {
    objectAdded(obj->metaObject());
  }
}

void ToolModel::objectAdded(const QMetaObject *mo)
{
  foreach (ToolFactory *factory, m_inactiveTools) {
    if (factory->supportedTypes().contains(mo->className())) {
      qDebug() << "found instance of class" << mo->className()
               << "activating tool" << factory->name();
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
