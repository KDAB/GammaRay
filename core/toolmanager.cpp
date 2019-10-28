/*
  toolmanager.cpp

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

#include "toolmanager.h"

#include "metaobject.h"
#include "metaobjectrepository.h"
#include "probe.h"
#include "probesettings.h"
#include "proxytoolfactory.h"
#include "toolfactory.h"

#include "tools/metatypebrowser/metatypebrowser.h"
#include "tools/objectinspector/objectinspector.h"
#include "tools/problemreporter/problemreporter.h"
#include "tools/resourcebrowser/resourcebrowser.h"
#include "tools/messagehandler/messagehandler.h"
#include "tools/metaobjectbrowser/metaobjectbrowser.h"

#include <compat/qasconst.h>

#include <QDebug>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QThread>

#include <algorithm>

using namespace GammaRay;

ToolManager::ToolManager(QObject *parent)
    : ToolManagerInterface(parent)
    , m_toolPluginManager(new ToolPluginManager(this))
{
    // built-in tools
    addToolFactory(new ObjectInspectorFactory(this));
    addToolFactory(new ResourceBrowserFactory(this));
    addToolFactory(new MetaObjectBrowserFactory(this));
    addToolFactory(new MetaTypeBrowserFactory(this));
    addToolFactory(new MessageHandlerFactory(this));
    addToolFactory(new ProblemReporterFactory(this));

    Q_FOREACH (ToolFactory *factory, m_toolPluginManager->plugins())
        addToolFactory(factory);
}

void ToolManager::selectObject(const ObjectId &id, const QString &toolId)
{
    switch (id.type()) {
    case ObjectId::Invalid:
        return;
    case ObjectId::QObjectType:
    {
        QMutexLocker lock(Probe::objectLock());
        if (!Probe::instance()->isValidObject(id.asQObject()))
            return;

        Probe::instance()->selectObject(id.asQObject(), toolId);
        break;
    }
    case ObjectId::VoidStarType:
        Probe::instance()->selectObject(id.asVoidStar(), id.typeName());
        break;
    }
}

void ToolManager::selectTool(const QString &toolId)
{
    emit toolSelected(toolId);
}

void ToolManager::requestToolsForObject(const ObjectId &id)
{
    QVector<QString> toolInfos;
    switch (id.type()) {
    case ObjectId::Invalid:
        return;
    case ObjectId::QObjectType:
    {
        QMutexLocker lock(Probe::objectLock());
        if (!Probe::instance()->isValidObject(id.asQObject()))
            return;

        toolInfos = toolsForObject(id.asQObject());
        break;
    }
    case ObjectId::VoidStarType:
        const auto asVoidStar = reinterpret_cast<void *>(id.id());
        toolInfos = toolsForObject(asVoidStar, id.typeName());
        break;
    }
    emit toolsForObjectResponse(id, toolInfos);
}

void ToolManager::requestAvailableTools()
{
    QVector<ToolData> toolInfos;
    toolInfos.reserve(m_tools.size());
    for (ToolFactory *factory : qAsConst(m_tools))
        toolInfos.push_back(toolInfoForFactory(factory));
    emit availableToolsResponse(toolInfos);
}

ToolData ToolManager::toolInfoForFactory(ToolFactory *factory) const
{
    ToolData info;
    info.id = factory->id();
    info.hasUi = !factory->isHidden();
    info.enabled = !m_disabledTools.contains(factory);
    return info;
}

bool ToolManager::hasTool(const QString &id) const
{
    for (ToolFactory *factory : m_tools) {
        if (factory->id() == id)
            return true;
    }
    return false;
}

QVector<QString> ToolManager::toolsForObject(QObject *object) const
{
    if (!object)
        return QVector<QString>();

    QVector<QString> ret;
    QSet<ToolFactory *> knownTools;
    const QMetaObject *metaObject = object->metaObject();
    while (metaObject) {
        for (auto factory : m_tools) {
            if (factory && !factory->isHidden() && factory->selectableTypes().contains(metaObject->className()) && !knownTools.contains(factory)) {
                knownTools << factory;
                ret.append(factory->id());
            }
        }
        metaObject = metaObject->superClass();
    }
    return ret;
}

QVector<QString> ToolManager::toolsForObject(const void *object, const QString &typeName) const
{
    if (!object)
        return QVector<QString>();

    QVector<QString> ret;
    const MetaObject *metaObject = MetaObjectRepository::instance()->metaObject(typeName);
    while (metaObject) {
        for (auto factory : m_tools) {
            if (factory && !factory->isHidden()
                && factory->selectableTypes().contains(metaObject->className().toUtf8()))
                ret.append(factory->id());
        }
        metaObject = metaObject->superClass();
    }
    return ret;
}

void ToolManager::objectAdded(QObject *obj)
{
    // note: hot path, don't do expensive operations here

    Q_ASSERT(QThread::currentThread() == thread());
    Q_ASSERT(Probe::instance()->isValidObject(obj));

    // m_knownMetaObjects allows us to skip the expensive recursive search for matching tools
    if (!m_knownMetaObjects.contains(obj->metaObject())) {
        objectAdded(obj->metaObject());
        m_knownMetaObjects.insert(obj->metaObject());
    }
}

void ToolManager::objectAdded(const QMetaObject *mo)
{
    // note: hot path, don't do expensive operations here

    Q_ASSERT(thread() == QThread::currentThread());
    // as plugins can depend on each other, start from the base classes
    if (mo->superClass())
        objectAdded(mo->superClass());

    for (auto it = m_disabledTools.begin(); it != m_disabledTools.end();) {
        ToolFactory *factory = *it;
        const auto begin = factory->supportedTypes().constBegin();
        const auto end = factory->supportedTypes().constEnd();
        if (std::find(begin, end, mo->className()) != end) {
            it = m_disabledTools.erase(it);
            factory->init(Probe::instance());
            emit toolEnabled(factory->id());
        } else {
            ++it;
        }
    }
}

void ToolManager::addToolFactory(ToolFactory *tool)
{
    const auto excludedTools = ProbeSettings::value(QStringLiteral("DisabledPlugins"), QString()).toString();
    if (excludedTools.split(QLatin1Char(';')).contains(tool->id())) {
        return;
    }

    m_tools.push_back(tool);
    m_disabledTools.insert(tool);
}

ToolPluginManager *ToolManager::toolPluginManager() const
{
    return m_toolPluginManager.data();
}
