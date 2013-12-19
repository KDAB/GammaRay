/*
  clienttoolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "clienttoolmodel.h"

#include <ui/tools/connectioninspector/connectioninspectorwidget.h>
#include <ui/tools/localeinspector/localeinspectorwidget.h>
#include <ui/tools/messagehandler/messagehandlerwidget.h>
#include <ui/tools/metaobjectbrowser/metaobjectbrowserwidget.h>
#include <ui/tools/metatypebrowser/metatypebrowserwidget.h>
#include <ui/tools/mimetypes/mimetypeswidget.h>
#include <ui/tools/modelinspector/modelinspectorwidget.h>
#include <ui/tools/objectinspector/objectinspectorwidget.h>
#include <ui/tools/resourcebrowser/resourcebrowserwidget.h>
#include <ui/tools/standardpaths/standardpathswidget.h>
#include <ui/tools/textdocumentinspector/textdocumentinspectorwidget.h>

#include <common/modelroles.h>
#include <common/pluginmanager.h>
#include <common/endpoint.h>
#include <ui/proxytooluifactory.h>

#include <QCoreApplication>
#include <QDir>
#include <QWidget>

using namespace GammaRay;


#define MAKE_FACTORY(type, remote) \
class type ## Factory : public ToolUiFactory { \
public: \
  virtual inline QString id() const { return "GammaRay::" #type; } \
  virtual inline QWidget *createWidget(QWidget *parentWidget) { return new type ## Widget(parentWidget); } \
  virtual inline bool remotingSupported() const { return remote; } \
}

MAKE_FACTORY(ConnectionInspector, true);
MAKE_FACTORY(LocaleInspector, true);
MAKE_FACTORY(MessageHandler, true);
MAKE_FACTORY(MetaObjectBrowser, true);
MAKE_FACTORY(MetaTypeBrowser, true);
MAKE_FACTORY(MimeTypes, true);
MAKE_FACTORY(ModelInspector, true);
MAKE_FACTORY(ObjectInspector, true);
MAKE_FACTORY(ResourceBrowser, true);
MAKE_FACTORY(StandardPaths, true);
MAKE_FACTORY(TextDocumentInspector, false);

ClientToolModel::ClientToolModel(QObject* parent) : QSortFilterProxyModel(parent)
{
  insertFactory(new ConnectionInspectorFactory);
  insertFactory(new LocaleInspectorFactory);
  insertFactory(new MessageHandlerFactory);
  insertFactory(new MetaObjectBrowserFactory);
  insertFactory(new MetaTypeBrowserFactory);
  insertFactory(new MimeTypesFactory);
  insertFactory(new ModelInspectorFactory);
  insertFactory(new ObjectInspectorFactory);
  insertFactory(new ResourceBrowserFactory);
  insertFactory(new StandardPathsFactory);
  insertFactory(new TextDocumentInspectorFactory);

  const QString pluginPath = QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String(GAMMARAY_RELATIVE_BIN_TO_PLUGIN_PATH);
  PluginManager<ToolUiFactory, ProxyToolUiFactory> pm(pluginPath);
  foreach(ToolUiFactory* factory, pm.plugins())
    insertFactory(factory);
}

ClientToolModel::~ClientToolModel()
{
  qDeleteAll(m_factories.values());
}

QVariant ClientToolModel::data(const QModelIndex& index, int role) const
{
  if (role == ToolModelRole::ToolFactory || role == ToolModelRole::ToolWidget || role == Qt::ToolTipRole) {
    const QString toolId = QSortFilterProxyModel::data(index, ToolModelRole::ToolId).toString();
    if (toolId.isEmpty())
      return QVariant();

    if (role == ToolModelRole::ToolFactory)
      return QVariant::fromValue(m_factories.value(toolId));
    if (role == ToolModelRole::ToolWidget) {
      const QHash<QString, QWidget*>::const_iterator it = m_widgets.constFind(toolId);
      if (it != m_widgets.constEnd())
        return QVariant::fromValue(it.value());
      ToolUiFactory *factory = m_factories.value(toolId);
      if (!factory)
        return QVariant();
      QWidget *widget = factory->createWidget(m_parentWidget);
      m_widgets.insert(toolId, widget);
      return QVariant::fromValue(widget);
    }
    if (role == Qt::ToolTipRole) {
      ToolUiFactory *factory = m_factories.value(toolId);
      if (factory && (!factory->remotingSupported() && Endpoint::instance()->isRemoteClient()))
        return tr("This tool does not work in out-of-process mode.");
    }
  }

  return QSortFilterProxyModel::data(index, role);
}

bool ClientToolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == ToolModelRole::ToolWidget) {
    const QString toolId = QSortFilterProxyModel::data(index, ToolModelRole::ToolId).toString();
    Q_ASSERT(!toolId.isEmpty());
    Q_ASSERT(!m_widgets.contains(toolId));
    m_widgets.insert(toolId, value.value<QWidget*>());
    return true;
  } else if (role == ToolModelRole::ToolWidgetParent) {
    m_parentWidget = value.value<QWidget*>();
    return true;
  }

  return QSortFilterProxyModel::setData(index, value, role);
}

Qt::ItemFlags ClientToolModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags ret = QSortFilterProxyModel::flags(index);
  const QString toolId = QSortFilterProxyModel::data(index, ToolModelRole::ToolId).toString();
  ToolUiFactory *factory = m_factories.value(toolId);
  if (!factory || (!factory->remotingSupported() && Endpoint::instance()->isRemoteClient())) {
    ret &= ~Qt::ItemIsEnabled;
  }
  return ret;
}

void ClientToolModel::insertFactory(ToolUiFactory* factory)
{
  m_factories.insert(factory->id(), factory);
}


