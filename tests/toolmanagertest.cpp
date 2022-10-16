/*
  toolmanagertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "baseprobetest.h"

#include <common/toolmanagerinterface.h>
#include <ui/clienttoolmanager.h>

#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QAbstractItemModel>
#include <QAction>
#include <QSignalSpy>
#include <QWidget>

using namespace GammaRay;

Q_DECLARE_METATYPE(QVector<GammaRay::ToolInfo>)

class ToolManagerTest : public BaseProbeTest
{
    Q_OBJECT
private:
    static int visibleRowCount(QAbstractItemModel *model)
    {
        int count = 0;
        for (int i = 0; i < model->rowCount(); ++i) {
            auto idx = model->index(i, 1);
            if (!idx.data(Qt::DisplayRole).toString().startsWith(QLatin1String("QDesktop")))
                ++count;
        }
        return count;
    }

private slots:
    static void initTestCase()
    {
        qRegisterMetaType<QVector<ToolInfo>>();
        new ClientToolManager;
    }

    void init()
    {
        delete Probe::instance();
        createProbe();
    }

    void testProbeSide()
    {
        auto *toolManager = ObjectBroker::object<ToolManagerInterface *>();
        QVERIFY(toolManager);

        QSignalSpy availableToolsSpy(toolManager, &ToolManagerInterface::availableToolsResponse);
        QSignalSpy toolEnabledSpy(toolManager, &ToolManagerInterface::toolEnabled);
        QSignalSpy toolSelectedSpy(toolManager, &ToolManagerInterface::toolSelected);
        QSignalSpy toolsForObjectSpy(toolManager, &ToolManagerInterface::toolsForObjectResponse);

        toolManager->requestAvailableTools();
        availableToolsSpy.wait(500);
        QCOMPARE(availableToolsSpy.size(), 1);
        const auto &list = availableToolsSpy[0][0].value<QVector<ToolData>>();
        QVERIFY(!list.isEmpty());

        bool hasBasicTools = false;
        const ToolData *actionInspector = nullptr;
        const ToolData *guiSupport = nullptr;
        for (const auto &tool : list) {
            if (tool.id == "GammaRay::ObjectInspector")
                hasBasicTools = true;
            else if (tool.id == "gammaray_actioninspector")
                actionInspector = &tool;
            else if (tool.id == "gammaray_guisupport")
                guiSupport = &tool;
        }
        QVERIFY(hasBasicTools);
        QVERIFY(actionInspector);
        QCOMPARE(actionInspector->enabled, false); /* coverity[UNINIT_CTOR] */
        QCOMPARE(actionInspector->hasUi, true); /* coverity[UNINIT_CTOR] */
        QVERIFY(guiSupport);
        QCOMPARE(guiSupport->enabled, true); /* coverity[UNINIT_CTOR] */
        QCOMPARE(guiSupport->hasUi, false); /* coverity[UNINIT_CTOR] */
        // Create QAction to enable action inspector
        QAction action("Test Action", this);
        toolEnabledSpy.wait(1000);
        QVERIFY(!toolEnabledSpy.isEmpty());
        QStringList enabledTools;
        for (auto i = toolEnabledSpy.constBegin(); i != toolEnabledSpy.constEnd(); ++i)
            enabledTools << i->first().toString();
        QVERIFY(enabledTools.contains("gammaray_actioninspector"));

        toolManager->selectObject(ObjectId(&action), QStringLiteral("gammaray_actioninspector"));
        toolSelectedSpy.wait(50);
        QCOMPARE(toolSelectedSpy.size(), 1);
        QString selectedTool = toolSelectedSpy.first().first().toString();
        QCOMPARE(selectedTool, QStringLiteral("gammaray_actioninspector"));

        toolManager->requestToolsForObject(ObjectId(&action));
        toolsForObjectSpy.wait(50);
        QCOMPARE(toolsForObjectSpy.size(), 1);
        const ObjectId &actionId = toolsForObjectSpy.first().first().value<ObjectId>();
        QCOMPARE(actionId.asQObject(), &action);
        const auto &actionTools = toolsForObjectSpy.first().last().value<QVector<QString>>();
        QStringList supportedToolIds;
        for (const auto &tool : actionTools)
            supportedToolIds << tool;
        QVERIFY(supportedToolIds.contains(QStringLiteral("GammaRay::ObjectInspector")));
        QVERIFY(supportedToolIds.contains(QStringLiteral("GammaRay::MetaObjectBrowser")));
        QVERIFY(supportedToolIds.contains(QStringLiteral("gammaray_actioninspector")));
    }

    void testClientSide()
    {
        ClientToolManager::instance()->requestAvailableTools();
        ModelTest modelTest(ClientToolManager::instance()->model());

        // we're testing inprocess, thus tool list should be available instantly.
        QVERIFY(ClientToolManager::instance()->isToolListLoaded());

        testHasBasicTools(false);
        testToolEnabled();
        testToolSelected();
        testRequestToolsForObject();

        testClearance();

        ClientToolManager::instance()->requestAvailableTools(); // "reconnect"
        testHasBasicTools(true);
        testToolSelected();
        testRequestToolsForObject();
    }

private:
    static void testHasBasicTools(bool actionInspectorEnabled)
    {
        bool hasBasicTools = false;
        const ToolInfo *actionInspector = nullptr;
        const ToolInfo *guiSupport = nullptr;
        foreach (const ToolInfo &tool, ClientToolManager::instance()->tools()) {
            if (tool.id() == "GammaRay::ObjectInspector")
                hasBasicTools = true;
            else if (tool.id() == "gammaray_actioninspector")
                actionInspector = &tool;
            else if (tool.id() == "gammaray_guisupport")
                guiSupport = &tool;
        }
        QVERIFY(hasBasicTools);
        QVERIFY(actionInspector);
        QCOMPARE(actionInspector->isEnabled(), actionInspectorEnabled);
        QCOMPARE(actionInspector->hasUi(), true);
        QVERIFY(!guiSupport); // tools without ui are supposed to be filtered out
        QVERIFY(!ClientToolManager::instance()->widgetForId("inexistantTool"));
        QVERIFY(actionInspectorEnabled == ( bool )ClientToolManager::instance()->widgetForId("gammaray_actioninspector")); // if tool is disabled we explicitly want widgetForId to be null.
    }

    void testToolEnabled()
    {
        QSignalSpy toolEnabledSpy(ClientToolManager::instance(), &ClientToolManager::toolEnabled);
        // Create QAction to enable action inspector
        QAction action("Test Action", this);
        toolEnabledSpy.wait(50);
        QVERIFY(!toolEnabledSpy.isEmpty());
        QStringList enabledTools;
        for (auto i = toolEnabledSpy.constBegin(); i != toolEnabledSpy.constEnd(); ++i)
            enabledTools << i->first().toString();
        QVERIFY(enabledTools.contains("gammaray_actioninspector"));

        QVERIFY(ClientToolManager::instance()->widgetForId("gammaray_actioninspector"));
    }

    void testToolSelected()
    {
        auto *toolManager = ObjectBroker::object<ToolManagerInterface *>();
        QVERIFY(toolManager);

        QSignalSpy toolSelectedSpy(ClientToolManager::instance(), &ClientToolManager::toolSelected);
        QAction action("Test Action", this);

        toolManager->selectObject(ObjectId(&action), QStringLiteral("gammaray_actioninspector"));
        toolSelectedSpy.wait(50);
        QCOMPARE(toolSelectedSpy.size(), 1);
        QString selectedTool = toolSelectedSpy.first().first().toString();
        QCOMPARE(selectedTool, QStringLiteral("gammaray_actioninspector"));
    }

    void testRequestToolsForObject()
    {
        QSignalSpy toolsForObjectSpy(ClientToolManager::instance(), &ClientToolManager::toolsForObjectResponse);
        QAction action("Test Action", this);

        ClientToolManager::instance()->requestToolsForObject(ObjectId(&action));
        toolsForObjectSpy.wait(50);
        QCOMPARE(toolsForObjectSpy.size(), 1);
        const ObjectId &actionId = toolsForObjectSpy.first().first().value<ObjectId>();
        QCOMPARE(actionId.asQObject(), &action);
        const auto &actionTools = toolsForObjectSpy.first().last().value<QVector<ToolInfo>>();
        QStringList supportedToolIds;
        for (const auto &tool : actionTools) {
            QVERIFY(!tool.name().isEmpty());
            QVERIFY(tool.name() != tool.id());
            supportedToolIds << tool.id();
        }
        QVERIFY(supportedToolIds.contains(QStringLiteral("GammaRay::ObjectInspector")));
        QVERIFY(supportedToolIds.contains(QStringLiteral("GammaRay::MetaObjectBrowser")));
        QVERIFY(supportedToolIds.contains(QStringLiteral("gammaray_actioninspector")));
    }

    static void testClearance()
    {
        QSignalSpy resetSpy(ClientToolManager::instance()->model(), &QAbstractItemModel::modelReset);
        ClientToolManager::instance()->clear();
        QVERIFY(ClientToolManager::instance());
        QCOMPARE(ClientToolManager::instance()->tools().size(), 0);
        resetSpy.wait(50);
        QCOMPARE(resetSpy.size(), 1);
    }
};

QTEST_MAIN(ToolManagerTest)

#include "toolmanagertest.moc"
