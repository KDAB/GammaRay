/*
  quickmaterialtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "basequicktest.h"

#include <plugins/quickinspector/materialextension/materialextensioninterface.h>
#include <core/propertycontroller.h>
#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QAbstractItemModel>
#include <QQuickItem>

#include <private/qquickitem_p.h>

using namespace GammaRay;

static QSGGeometryNode *findGeometryNode(QSGNode *node)
{
    while (node) {
        if (node->type() == QSGNode::GeometryNodeType)
            return static_cast<QSGGeometryNode *>(node);
        if (node->childCount() == 0)
            return nullptr;
        if (node->childCount() > 1 && node->firstChild()->type() != QSGNode::GeometryNodeType)
            return nullptr;
        node = node->firstChild();
    }

    return nullptr;
}

class QuickMaterialTest : public BaseQuickTest
{
    Q_OBJECT
private slots:
    void testStaticShader()
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFAIL("QuickMaterial hasn't been ported to Qt6 yet");
        return;
#endif

        if (!showSource("qrc:/manual/shadereffect.qml"))
            return;

        auto iface = ObjectBroker::object<MaterialExtensionInterface *>("com.kdab.GammaRay.QuickSceneGraph.material");
        QVERIFY(iface);
        QSignalSpy shaderSpy(iface, SIGNAL(gotShader(QString)));
        QVERIFY(shaderSpy.isValid());

        auto propModel = ObjectBroker::model("com.kdab.GammaRay.QuickSceneGraph.materialPropertyModel");
        QVERIFY(propModel);
        ModelTest propModelTest(propModel);
        QCOMPARE(propModel->rowCount(), 0);

        auto shaderModel = ObjectBroker::model("com.kdab.GammaRay.QuickSceneGraph.shaderModel");
        QVERIFY(shaderModel);
        ModelTest shaderModelTest(shaderModel);
        QCOMPARE(shaderModel->rowCount(), 0);

        auto imageItem = view()->rootObject();
        QVERIFY(imageItem);
        QCOMPARE(imageItem->metaObject()->className(), "QQuickImage");
        Probe::instance()->selectObject(imageItem, QPoint());

        auto obj = ObjectBroker::object<PropertyControllerInterface *>("com.kdab.GammaRay.QuickSceneGraph.controller");
        auto controller = qobject_cast<PropertyController *>(obj);
        QVERIFY(controller);

        auto priv = QQuickItemPrivate::get(imageItem);
        QVERIFY(priv->itemNodeInstance);
        auto geometryNode = findGeometryNode(priv->itemNodeInstance);
        QVERIFY(geometryNode);
        controller->setObject(geometryNode, "QSGGeometryNode");

        QVERIFY(propModel->rowCount() > 1);
        QCOMPARE(shaderModel->rowCount(), 2);

        iface->getShader(0);
        QCOMPARE(shaderSpy.size(), 1);
        QVERIFY(!shaderSpy.at(0).at(0).toString().isEmpty());
        shaderSpy.clear();

        iface->getShader(1);
        QCOMPARE(shaderSpy.size(), 1);
        QVERIFY(!shaderSpy.at(0).at(0).toString().isEmpty());

        controller->setObject(nullptr, QString());
    }

    void testDynamicShader()
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFAIL("QuickMaterial hasn't been ported to Qt6 yet");
        return;
#endif
        if (!showSource("qrc:/manual/shadereffect.qml"))
            return;

        auto iface = ObjectBroker::object<MaterialExtensionInterface *>("com.kdab.GammaRay.QuickSceneGraph.material");
        QVERIFY(iface);
        QSignalSpy shaderSpy(iface, SIGNAL(gotShader(QString)));
        QVERIFY(shaderSpy.isValid());

        auto propModel = ObjectBroker::model("com.kdab.GammaRay.QuickSceneGraph.materialPropertyModel");
        QVERIFY(propModel);
        ModelTest propModelTest(propModel);
        QCOMPARE(propModel->rowCount(), 0);

        auto shaderModel = ObjectBroker::model("com.kdab.GammaRay.QuickSceneGraph.shaderModel");
        QVERIFY(shaderModel);
        ModelTest shaderModelTest(shaderModel);
        QCOMPARE(shaderModel->rowCount(), 0);

        QQuickItem *effectItem = nullptr;
        foreach (auto item, view()->rootObject()->findChildren<QQuickItem *>()) {
            if (item->inherits("QQuickShaderEffect"))
                effectItem = item;
        }
        QVERIFY(effectItem);
        Probe::instance()->selectObject(effectItem, QPoint());

        auto obj = ObjectBroker::object<PropertyControllerInterface *>("com.kdab.GammaRay.QuickSceneGraph.controller");
        auto controller = qobject_cast<PropertyController *>(obj);
        QVERIFY(controller);

        auto priv = QQuickItemPrivate::get(effectItem);
        QVERIFY(priv->itemNodeInstance);
        auto geometryNode = findGeometryNode(priv->itemNodeInstance);
        QVERIFY(geometryNode);
        controller->setObject(geometryNode, "QSGGeometryNode");

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
        QVERIFY(propModel->rowCount() > 1);
#endif
        QCOMPARE(shaderModel->rowCount(), 2);

        iface->getShader(0);
        QCOMPARE(shaderSpy.size(), 1);
        QVERIFY(!shaderSpy.at(0).at(0).toString().isEmpty());
        QVERIFY(shaderSpy.at(0).at(0).toString().contains(QLatin1String("TESTVERTEXSHADER")));
        shaderSpy.clear();

        iface->getShader(1);
        QCOMPARE(shaderSpy.size(), 1);
        QVERIFY(!shaderSpy.at(0).at(0).toString().isEmpty());
        QVERIFY(shaderSpy.at(0).at(0).toString().contains(QLatin1String("TESTFRAGMENTSHADER")));

        controller->setObject(nullptr, QString());
    }
};

QTEST_MAIN(QuickMaterialTest)

#include "quickmaterialtest.moc"
