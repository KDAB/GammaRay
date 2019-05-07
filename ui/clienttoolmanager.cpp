/*
  clienttoolmanager.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "clienttoolmanager.h"
#include "clienttoolmodel.h"

#include <ui/proxytooluifactory.h>
#include <ui/tools/messagehandler/messagehandlerwidget.h>
#include <ui/tools/metaobjectbrowser/metaobjectbrowserwidget.h>
#include <ui/tools/metatypebrowser/metatypebrowserwidget.h>
#include <ui/tools/objectinspector/objectinspectorwidget.h>
#include <ui/tools/problemreporter/problemreporterwidget.h>
#include <ui/tools/resourcebrowser/resourcebrowserwidget.h>

#include <common/endpoint.h>
#include <common/modelroles.h>
#include <common/objectbroker.h>
#include <common/pluginmanager.h>
#include <common/toolmanagerinterface.h>

#include <QCoreApplication>
#include <QDir>
#include <QWidget>

#include <algorithm>

using namespace GammaRay;

#define MAKE_FACTORY(type, label) \
    class type##Factory : public ToolUiFactory { \
    public: \
        virtual QString id() const override { return "GammaRay::" #type; } \
        virtual QString name() const override { return label; } \
        virtual QWidget *createWidget(QWidget *parentWidget) override  { \
            return new type##Widget(parentWidget); \
        } \
    }

MAKE_FACTORY(MessageHandler,    qApp->translate("GammaRay::MessageHandlerFactory", "Messages"));
MAKE_FACTORY(MetaObjectBrowser, qApp->translate("GammaRay::MetaObjectBrowserFactory", "Meta Objects"));
MAKE_FACTORY(MetaTypeBrowser,   qApp->translate("GammaRay::MetaTypeBrowserFactory", "Meta Types"));
MAKE_FACTORY(ProblemReporter,   qApp->translate("GammaRay::ProblemReporterFactory", "Problems"));
MAKE_FACTORY(ResourceBrowser,   qApp->translate("GammaRay::ResourceBrowserFactory", "Resources"));

struct PluginRepository {
    PluginRepository() = default;
    Q_DISABLE_COPY(PluginRepository)
    ~PluginRepository()
    {
        qDeleteAll(factories);
    }

    // ToolId -> ToolUiFactory
    QHash<QString, ToolUiFactory *> factories;
    // so far unused tools that yet have to be loaded/initialized
    QSet<ToolUiFactory *> uninitializedTools;
};

Q_GLOBAL_STATIC(PluginRepository, s_pluginRepository)

static void insertFactory(ToolUiFactory *factory)
{
    s_pluginRepository()->factories.insert(factory->id(), factory);
    s_pluginRepository()->uninitializedTools.insert(factory);
}

static void initPluginRepository()
{
    if (!s_pluginRepository()->factories.isEmpty())
        return;

    insertFactory(new MessageHandlerFactory);
    insertFactory(new MetaObjectBrowserFactory);
    insertFactory(new MetaTypeBrowserFactory);
    insertFactory(new ObjectInspectorFactory);
    insertFactory(new ProblemReporterFactory);
    insertFactory(new ResourceBrowserFactory);

    PluginManager<ToolUiFactory, ProxyToolUiFactory> pm;
    foreach (ToolUiFactory *factory, pm.plugins())
        insertFactory(factory);
}

static bool toolLessThan(const ToolInfo &lhs, const ToolInfo &rhs)
{
    return lhs.name().localeAwareCompare(rhs.name()) < 0;
}

ToolInfo::ToolInfo(const ToolData &toolData, ToolUiFactory *factory) :
    m_toolId(toolData.id),
    m_isEnabled(toolData.enabled),
    m_hasUi(toolData.hasUi),
    m_factory(factory)
{
}

ToolInfo::~ToolInfo() = default;

QString ToolInfo::id() const
{
    return m_toolId;
}

bool ToolInfo::isEnabled() const
{
    return m_isEnabled;
}

void ToolInfo::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

bool ToolInfo::hasUi() const
{
    return m_hasUi;
}

QString ToolInfo::name() const
{
    if (!m_factory)
        return m_toolId;
    return m_factory->name();
}

bool ToolInfo::remotingSupported() const
{
    return m_factory && m_factory->remotingSupported();
}

bool ToolInfo::isValid() const
{
    return !m_toolId.isEmpty();
}


ClientToolManager* ClientToolManager::s_instance = nullptr;

ClientToolManager::ClientToolManager(QObject *parent)
    : QObject(parent)
    , m_parentWidget(nullptr)
    , m_model(nullptr)
    , m_selectionModel(nullptr)
{
    Q_ASSERT(!s_instance);
    s_instance = this;

    initPluginRepository();

    connect(Endpoint::instance(), &Endpoint::disconnected, this, &ClientToolManager::clear);
    connect(Endpoint::instance(), &Endpoint::connectionEstablished, this, &ClientToolManager::requestAvailableTools);
}

ClientToolManager::~ClientToolManager()
{
    for (auto it = m_widgets.constBegin(); it != m_widgets.constEnd(); ++it)
        delete it.value().data();
    s_instance = nullptr;
}

void ClientToolManager::clear()
{
    emit aboutToReset();
    for (auto it = m_widgets.constBegin(); it != m_widgets.constEnd(); ++it)
        delete it.value().data();
    m_tools.clear();
    if (m_remote)
        disconnect(m_remote, nullptr, this, nullptr);
    m_remote = nullptr;
    emit reset();
}

void ClientToolManager::requestAvailableTools()
{
    m_remote = ObjectBroker::object<ToolManagerInterface *>();

    connect(m_remote.data(), &ToolManagerInterface::availableToolsResponse,
            this, &ClientToolManager::gotTools);
    connect(m_remote.data(), &ToolManagerInterface::toolEnabled,
            this, &ClientToolManager::toolGotEnabled);
    connect(m_remote.data(), &ToolManagerInterface::toolSelected,
            this, &ClientToolManager::toolGotSelected);
    connect(m_remote.data(), &ToolManagerInterface::toolsForObjectResponse,
            this, &ClientToolManager::toolsForObjectReceived);

    m_remote->requestAvailableTools();
}

void ClientToolManager::setToolParentWidget(QWidget *parent)
{
    m_parentWidget = parent;
}

QWidget *ClientToolManager::widgetForId(const QString &toolId) const
{
    return widgetForIndex(toolIndexForToolId(toolId));
}

QWidget *ClientToolManager::widgetForIndex(int index) const
{
    if (index < 0 || index >= m_tools.size())
        return nullptr;
    const ToolInfo &tool = m_tools.at(index);
    if (!tool.isEnabled())
        return nullptr;
    auto it = m_widgets.constFind(tool.id());
    if (it != m_widgets.constEnd() && it.value())
        return it.value();
    ToolUiFactory *factory = s_pluginRepository()->factories.value(tool.id());
    if (!factory)
        return nullptr;
    if (s_pluginRepository()->uninitializedTools.contains(factory)) {
        factory->initUi();
        s_pluginRepository()->uninitializedTools.remove(factory);
    }
    QWidget *widget = factory->createWidget(m_parentWidget);
    m_widgets.insert(tool.id(), widget);
    return widget;
}

void ClientToolManager::gotTools(const QVector<GammaRay::ToolData> &tools)
{
    emit aboutToReceiveData();
    for (const auto &tool : tools) {
        ToolUiFactory *factory = s_pluginRepository()->factories.value(tool.id);
        // hide tools we have no UI plugin for
        if (tool.hasUi && factory) {
            m_tools.append(ToolInfo(tool, factory));
        }
        if (tool.enabled) {
            if (factory && (factory->remotingSupported() || !Endpoint::instance()->isRemoteClient())
                && s_pluginRepository()->uninitializedTools.contains(factory)) {
                factory->initUi();
                s_pluginRepository()->uninitializedTools.remove(factory);
            }
        }
    }
    std::sort(m_tools.begin(), m_tools.end(), toolLessThan);
    emit toolListAvailable();

    if (m_remote) {
        disconnect(m_remote.data(), &ToolManagerInterface::availableToolsResponse,
                   this, &ClientToolManager::gotTools);
    }
}

bool ClientToolManager::isToolListLoaded() const
{
    return m_tools.count();
}

QAbstractItemModel *ClientToolManager::model()
{
    if (!m_model)
        m_model = new ClientToolModel(this);
    return m_model;
}

QItemSelectionModel *ClientToolManager::selectionModel()
{
    if (!m_selectionModel)
        m_selectionModel = new ClientToolSelectionModel(this);
    return m_selectionModel;
}

void ClientToolManager::requestToolsForObject(const ObjectId &id)
{
    if (!m_remote) {
        return;
    }
    m_remote->requestToolsForObject(id);
}

void ClientToolManager::selectObject(const ObjectId &id, const ToolInfo &toolInfo)
{
    if (!m_remote) {
        return;
    }
    m_remote->selectObject(id, toolInfo.id());
}

void ClientToolManager::toolsForObjectReceived(const ObjectId &id, const QVector<QString> &toolIds)
{
    QVector<ToolInfo> t;
    t.reserve(toolIds.size());
    for (const auto &toolId : toolIds) {
        const auto i = toolIndexForToolId(toolId);
        if (i >= 0)
            t.push_back(m_tools.at(i));
    }
    emit toolsForObjectResponse(id, t);
}

ClientToolManager *ClientToolManager::instance()
{
    return s_instance;
}

void ClientToolManager::toolGotEnabled(const QString &toolId)
{
    int i = 0;
    auto it = m_tools.begin();
    for (; it != m_tools.end(); i++, it++) {
        if (it->id() == toolId) {
            it->setEnabled(true);

            ToolUiFactory *factory = s_pluginRepository()->factories.value(it->id());
            if (factory && (factory->remotingSupported() || !Endpoint::instance()->isRemoteClient())
                && s_pluginRepository()->uninitializedTools.contains(factory)) {
                factory->initUi();
                s_pluginRepository()->uninitializedTools.remove(factory);
            }

            emit toolEnabled(toolId);
            emit toolEnabledByIndex(i);
        }
    }
}

void ClientToolManager::toolGotSelected(const QString &toolId)
{
    emit toolSelected(toolId);
    emit toolSelectedByIndex(toolIndexForToolId(toolId));
}

int ClientToolManager::toolIndexForToolId(const QString &toolId) const
{
    int i = 0;
    for (auto it = m_tools.constBegin(); it != m_tools.constEnd(); ++i, ++it) {
        if (it->id() == toolId)
            return i;
    }
    return -1;
}

ToolInfo ClientToolManager::toolForToolId(const QString &toolId) const {
    const int index = toolIndexForToolId(toolId);
    if (index < 0 || index >= m_tools.size())
        return ToolInfo();
    return m_tools.at(index);
}
