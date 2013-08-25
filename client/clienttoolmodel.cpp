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

#include "clienttoolmodel.h"

#include <ui/tools/codecbrowser/codecbrowserwidget.h>
#include <ui/tools/connectioninspector/connectioninspectorwidget.h>
#include <ui/tools/localeinspector/localeinspectorwidget.h>
#include <ui/tools/messagehandler/messagehandlerwidget.h>
#include <ui/tools/metaobjectbrowser/metaobjectbrowserwidget.h>
#include <ui/tools/metatypebrowser/metatypebrowserwidget.h>
#include <ui/tools/mimetypes/mimetypeswidget.h>
#include <ui/tools/modelinspector/modelinspectorwidget.h>
#include <ui/tools/objectinspector/objectinspectorwidget.h>
#include <ui/tools/resourcebrowser/resourcebrowserwidget.h>
#include <ui/tools/selectionmodelinspector/selectionmodelinspectorwidget.h>
#include <ui/tools/standardpaths/standardpathswidget.h>
#include <ui/tools/textdocumentinspector/textdocumentinspectorwidget.h>
#include <ui/tools/fontbrowser/fontbrowserwidget.h>

#include <common/pluginmanager.h>
#include <network/modelroles.h>
#include <include/toolfactory.h>

#include <QWidget>

using namespace GammaRay;


#define MAKE_FACTORY(type) \
class type ## Factory : public ToolFactory { \
public: \
  virtual inline QStringList supportedTypes() const { return QStringList(#type); } \
  virtual inline QString id() const { return "GammaRay::" #type; } \
  virtual inline QString name() const { return QString(); } \
  virtual inline void init(ProbeInterface *) {} \
  virtual inline QWidget *createWidget(ProbeInterface *, QWidget *parentWidget) { return new type ## Widget(parentWidget); } \
}

MAKE_FACTORY(CodecBrowser);
MAKE_FACTORY(ConnectionInspector);
MAKE_FACTORY(LocaleInspector);
MAKE_FACTORY(MessageHandler);
MAKE_FACTORY(MetaObjectBrowser);
MAKE_FACTORY(MetaTypeBrowser);
MAKE_FACTORY(MimeTypes);
MAKE_FACTORY(ModelInspector);
MAKE_FACTORY(ObjectInspector);
MAKE_FACTORY(ResourceBrowser);
MAKE_FACTORY(SelectionModelInspector);
MAKE_FACTORY(StandardPaths);
MAKE_FACTORY(TextDocumentInspector);
MAKE_FACTORY(FontBrowser);


ClientToolModel::ClientToolModel(QObject* parent) : RemoteModel(QLatin1String("com.kdab.GammaRay.ToolModel"), parent)
{
  insertFactory(new CodecBrowserFactory);
  insertFactory(new ConnectionInspectorFactory);
  insertFactory(new LocaleInspectorFactory);
  insertFactory(new MessageHandlerFactory);
  insertFactory(new MetaObjectBrowserFactory);
  insertFactory(new MetaTypeBrowserFactory);
  insertFactory(new MimeTypesFactory);
  insertFactory(new ModelInspectorFactory);
  insertFactory(new ObjectInspectorFactory);
  insertFactory(new ResourceBrowserFactory);
  insertFactory(new SelectionModelInspectorFactory);
  insertFactory(new StandardPathsFactory);
  insertFactory(new TextDocumentInspectorFactory);
  insertFactory(new FontBrowserFactory);

  foreach(ToolFactory* factory, PluginManager::instance()->plugins())
    insertFactory(factory);
}

ClientToolModel::~ClientToolModel()
{
  qDeleteAll(m_factories.values());
}

QVariant ClientToolModel::data(const QModelIndex& index, int role) const
{
  if (role == ToolModelRole::ToolFactory || role == ToolModelRole::ToolWidget) {
    const QString toolId = RemoteModel::data(index, ToolModelRole::ToolId).toString();
    if (toolId.isEmpty())
      return QVariant();

    if (role == ToolModelRole::ToolFactory)
      return QVariant::fromValue(m_factories.value(toolId));
    if (role == ToolModelRole::ToolWidget) {
      const QHash<QString, QWidget*>::const_iterator it = m_widgets.constFind(toolId);
      if (it != m_widgets.constEnd())
        return QVariant::fromValue(it.value());
      ToolFactory *factory = m_factories.value(toolId);
      if (!factory)
        return QVariant();
      QWidget *widget = factory->createWidget(0, m_parentWidget);
      m_widgets.insert(toolId, widget);
      return QVariant::fromValue(widget);
    }
  }

  return RemoteModel::data(index, role);
}

bool ClientToolModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (index.isValid() && role == ToolModelRole::ToolWidget) {
    const QString toolId = RemoteModel::data(index, ToolModelRole::ToolId).toString();
    Q_ASSERT(!toolId.isEmpty());
    Q_ASSERT(!m_widgets.contains(toolId));
    m_widgets.insert(toolId, value.value<QWidget*>());
    return true;
  } else if (role == ToolModelRole::ToolWidgetParent) {
    m_parentWidget = value.value<QWidget*>();
    return true;
  }

  return RemoteModel::setData(index, value, role);
}

void ClientToolModel::insertFactory(ToolFactory* factory)
{
  m_factories.insert(factory->id(), factory);
}


#include "clienttoolmodel.moc"
