/*
  clienttoolmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
MAKE_FACTORY(ResourceBrowser, true);
MAKE_FACTORY(StandardPaths, true);
MAKE_FACTORY(TextDocumentInspector, true);

struct PluginRepository {
    ~PluginRepository() {
        qDeleteAll(factories.values());
    }

    // ToolId -> ToolUiFactory
    QHash<QString, ToolUiFactory*> factories;
    // so far unused tools that yet have to be loaded/initialized
    QSet<ToolUiFactory*> inactiveTools;
};

Q_GLOBAL_STATIC(PluginRepository, s_pluginRepository)

static void insertFactory(ToolUiFactory* factory)
{
    s_pluginRepository()->factories.insert(factory->id(), factory);
    s_pluginRepository()->inactiveTools.insert(factory);
}

static void initPluginRepository()
{
    if (!s_pluginRepository()->factories.isEmpty())
        return;

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

    PluginManager<ToolUiFactory, ProxyToolUiFactory> pm;
    foreach(ToolUiFactory* factory, pm.plugins())
        insertFactory(factory);
}


ClientToolModel::ClientToolModel(QObject* parent) : QSortFilterProxyModel(parent)
{
  initPluginRepository();
  connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateToolInitialization(QModelIndex,QModelIndex)));
}

ClientToolModel::~ClientToolModel()
{
}

QVariant ClientToolModel::data(const QModelIndex& index, int role) const
{
  if (role == ToolModelRole::ToolFactory || role == ToolModelRole::ToolWidget || role == Qt::ToolTipRole) {
    const QString toolId = QSortFilterProxyModel::data(index, ToolModelRole::ToolId).toString();
    if (toolId.isEmpty())
      return QVariant();

    if (role == ToolModelRole::ToolFactory)
      return QVariant::fromValue(s_pluginRepository()->factories.value(toolId));
    if (role == ToolModelRole::ToolWidget) {
      const QHash<QString, QWidget*>::const_iterator it = m_widgets.constFind(toolId);
      if (it != m_widgets.constEnd())
        return QVariant::fromValue(it.value());
      ToolUiFactory *factory = s_pluginRepository()->factories.value(toolId);
      if (!factory)
        return QVariant();
      if (s_pluginRepository()->inactiveTools.contains(factory)) {
        factory->initUi();
        s_pluginRepository()->inactiveTools.remove(factory);
      }
      QWidget *widget = factory->createWidget(m_parentWidget);
      m_widgets.insert(toolId, widget);
      return QVariant::fromValue(widget);
    }
    if (role == Qt::ToolTipRole) {
      ToolUiFactory *factory = s_pluginRepository()->factories.value(toolId);
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
  ToolUiFactory *factory = s_pluginRepository()->factories.value(toolId);
  if (!factory || (!factory->remotingSupported() && Endpoint::instance()->isRemoteClient())) {
    ret &= ~Qt::ItemIsEnabled;
  }
  return ret;
}

void ClientToolModel::updateToolInitialization(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
    QModelIndex index = QSortFilterProxyModel::index(i, 0);

    if (QSortFilterProxyModel::data(index, ToolModelRole::ToolEnabled).toBool()) {
      const QString toolId = QSortFilterProxyModel::data(index, ToolModelRole::ToolId).toString();
      ToolUiFactory *factory = s_pluginRepository()->factories.value(toolId);

      if (factory && (factory->remotingSupported() || !Endpoint::instance()->isRemoteClient()) && s_pluginRepository()->inactiveTools.contains(factory)) {
        factory->initUi();
        s_pluginRepository()->inactiveTools.remove(factory);
      }
    }
  }
}
