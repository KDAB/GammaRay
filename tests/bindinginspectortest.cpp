/*
  bindinginspectortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include <core/probe.h>
#include <common/paths.h>
#include <common/objectbroker.h>
#include <probe/hooks.h>
#include <probe/probecreator.h>

#include <core/tools/bindinginspector/bindingnode.h>
#include <core/tools/bindinginspector/qmlbindingmodel.h>
#include <core/tools/bindinginspector/abstractbindingprovider.h>
#include <plugins/qmlsupport/qmlbindingprovider.h>
#include <plugins/quickinspector/quickimplicitbindingdependencyprovider.h>

#include <3rdparty/qt/modeltest.h>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <private/qquicktext_p.h>
#include <private/qquickrectangle_p.h>

#include <QDebug>
#include <QTest>
#include <QObject>
#include <QThread>
#include <QSignalSpy>

using namespace GammaRay;

class MockBindingProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) override
    {
        std::vector<std::unique_ptr<BindingNode>> nodes;
        for (auto &&node : data) {
            if (node.parent == qintptr(obj)) {
                auto binding = new BindingNode(obj, node.propertyIndex);
                binding->setId(node.id);
                nodes.push_back(std::unique_ptr<BindingNode>(binding));
            }
        }
        return nodes;
    }

    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) override
    {
        std::vector<std::unique_ptr<BindingNode>> nodes;
        for (auto &&node : data) {
            if (node.parent == binding->id()) {
                auto dependency = new BindingNode(node.object, node.propertyIndex, binding);
                dependency->setId(node.id);
                nodes.push_back(std::unique_ptr<BindingNode>(dependency));
            }
        }
        return nodes;
    }

    QString canonicalNameFor(BindingNode *binding) override
    {
        return {};
    }

    bool canProvideBindingsFor(QObject *object) override
    {
        return true;
    }

    struct NodeData {
        NodeData(qintptr parent_, QObject *obj_, int index_, qintptr id_)
        : parent(parent_)
        , object(obj_)
        , propertyIndex(index_)
        , id(id_)
        {}

        qintptr parent;
        QObject *object;
        int propertyIndex;
        qintptr id;
    };
    std::vector<NodeData> data;
};

class MockObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a WRITE setA NOTIFY aChanged)
    Q_PROPERTY(bool b READ b WRITE setB NOTIFY bChanged)
    Q_PROPERTY(char c READ c WRITE setC NOTIFY cChanged)
    Q_PROPERTY(double d READ d WRITE setD NOTIFY dChanged)
    Q_PROPERTY(QString e READ e WRITE setE NOTIFY eChanged)

public:
    MockObject (int a, bool b, char c, double d, const QString &e)
        : QObject(0)
        , m_a(a)
        , m_b(b)
        , m_c(c)
        , m_d(d)
        , m_e(e)
    {}

    int a() const { return m_a; }
    bool b() const { return m_b; }
    char c() const { return m_c; }
    double d() const { return m_d; }
    const QString &e() const { return m_e; }

    void setA(int a) { m_a = a;     emit aChanged(); }
    void setB(bool b) { m_b = b;    emit bChanged(); }
    void setC(char c) { m_c = c;    emit cChanged(); }
    void setD(double d) { m_d = d;  emit dChanged(); }
    void setE(QString e) { m_e = e; emit eChanged(); }

signals:
    void aChanged();
    void bChanged();
    void cChanged();
    void dChanged();
    void eChanged();

private:
    int m_a;
    bool m_b;
    char m_c;
    double m_d;
    QString m_e;
};

class BindingInspectorTest : public QObject
{
    Q_OBJECT
    int m_foo = 0;
private:
    void createProbe()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        Hooks::installHooks();
        Probe::startupHookReceived();
//         Q_ASSERT(++m_foo <= 1);
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
    }

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void testMockProvider();
    void testQmlBindingProvider_data();
    void testQmlBindingProvider();
    void testQtQuickProvider_data();
    void testQtQuickProvider();
    void testModel();
    void testModelDataChanged();
    void testModelAdditions();
    void testModelInsertions();
    void testModelRemovalAtEnd();
    void testModelRemovalInside();
    void testIntegration();

private:
    QAbstractItemModel *bindingModel;
    MockBindingProvider *provider;
    ModelTest *modelTest;
};


void BindingInspectorTest::initTestCase()
{
    QQmlEngine engine; // Needed to initialize the Qml support plugin
    provider = new MockBindingProvider;
    QmlBindingModel::registerBindingProvider(std::unique_ptr<MockBindingProvider>(provider));
}

void BindingInspectorTest::init()
{
    createProbe();
}

void BindingInspectorTest::cleanup()
{
}

void BindingInspectorTest::testMockProvider()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };
    provider->data = {{
        // parent-id/obj, object, propertyIndex,                    , id
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 2 },
        { qintptr(&obj2), &obj2, MockObject::staticMetaObject.indexOfProperty("b"), 3 },
        { 2,              &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 4 },
        { 2,              &obj2, MockObject::staticMetaObject.indexOfProperty("b"), 5 },
        { 5,              &obj2, MockObject::staticMetaObject.indexOfProperty("a"), 6 },
        { 6,              &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 7 },
    }};

    auto bindings1 = provider->findBindingsFor(&obj1);
    auto &&bindingNode1 = bindings1.front();

    QVERIFY(bindingNode1);
    QCOMPARE(bindingNode1->parent(), nullptr);
    QCOMPARE(bindingNode1->object(), &obj1);
    QCOMPARE(bindingNode1->property().name(), "a");
    QCOMPARE(bindingNode1->isActive(), true);
    QCOMPARE(bindingNode1->isBindingLoop(), false);
    QCOMPARE(bindingNode1->cachedValue(), 53);

    auto dependencies1 = provider->findDependenciesFor(bindingNode1.get());
    QCOMPARE(dependencies1.size(), 0);

    auto bindings2 = provider->findBindingsFor(&obj1);
    auto &&bindingNode2 = bindings2.back();

    QVERIFY(bindingNode2);
    QCOMPARE(bindingNode2->parent(), nullptr);
    QCOMPARE(bindingNode2->object(), &obj1);
    QCOMPARE(bindingNode2->property().name(), "c");
    QCOMPARE(bindingNode2->isActive(), true);
    QCOMPARE(bindingNode2->isBindingLoop(), false);
    QCOMPARE(bindingNode2->cachedValue(), 'x');

    auto dependencies2 = provider->findDependenciesFor(bindingNode2.get());
    auto &&dependency2 = dependencies2.front();
    QCOMPARE(dependencies2.size(), 2);
    QCOMPARE(dependency2->parent(), bindingNode2.get());
    QCOMPARE(dependency2->object(), &obj1);
    QCOMPARE(dependency2->property().name(), "b");
    QCOMPARE(dependency2->isActive(), true);
    QCOMPARE(dependency2->isBindingLoop(), false);
    QCOMPARE(dependency2->cachedValue(), true);
    QCOMPARE(dependency2->dependencies().size(), 0);
}

void BindingInspectorTest::testQmlBindingProvider_data()
{
    QTest::addColumn<QByteArray>("code");
    QTest::addColumn<QString>("referencedObjectName");

    QTest::newRow("context property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"Hello world!\"\n"
                        "Text { objectName: 'text'; text: labelText }\n"
                    "}") << "rect";

    QTest::newRow("scope property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"I am wrong!\"\n"
                        "Text {\n"
                            "id: text\n"
                            "objectName: 'text'\n"
                            "property string labelText: \"Hello world!\"\n"
                            "text: labelText\n"
                        "}\n"
                    "}") << "text";

    QTest::newRow("id object property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"Hello world!\"\n"
                        "Text { objectName: 'text'; text: rect.labelText }\n"
                    "}") << "rect";

    QTest::newRow("dynamic context property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"Hello world!\"\n"
                        "Text { objectName: 'text'; Component.onCompleted: text = Qt.binding(function() { return labelText; }); }\n"
                    "}") << "rect";

    QTest::newRow("dynamic scope property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"I am wrong!\"\n"
                        "Text {\n"
                            "id: txt\n"
                            "objectName: 'txt'\n"
                            "property string labelText: \"Hello world!\"\n"
                            "Component.onCompleted: text = Qt.binding(function() { return labelText; });\n"
                        "}\n"
                    "}") << "txt";

    QTest::newRow("dynamic id object property")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "property string labelText: \"Hello world!\"\n"
                        "Text { objectName: 'text'; Component.onCompleted: text = Qt.binding(function() { return rect.labelText; }); }\n"
                    "}") << "rect";
}

void BindingInspectorTest::testQmlBindingProvider()
{
    QFETCH(QByteArray, code);
    QFETCH(QString, referencedObjectName);

    QQmlEngine engine;
    QQmlComponent c(&engine);
    c.setData(code, QUrl());
    QObject *rect = c.create();
    QTest::qWait(10);
    QVERIFY(rect);
    QObject *text = rect->findChildren<QQuickText *>().front();

    QObject *referencedObject = rect->objectName() == referencedObjectName ? rect : rect->findChild<QObject *>(referencedObjectName);
    QVERIFY(referencedObject);

    QmlBindingProvider provider;
    auto bindingNodes = provider.findBindingsFor(text);
    auto &&bindingNode = bindingNodes.front();

    QVERIFY(bindingNode);
    QCOMPARE(bindingNode->object(), text);
    QCOMPARE(bindingNode->property().name(), "text");
    QCOMPARE(bindingNode->isActive(), true);
    QCOMPARE(bindingNode->isBindingLoop(), false);
    QCOMPARE(bindingNode->cachedValue(), QStringLiteral("Hello world!"));

    auto dependencies = provider.findDependenciesFor(bindingNode.get());
    QCOMPARE(dependencies.size(), 1);
    const std::unique_ptr<BindingNode> &dependency = dependencies.front();
    QCOMPARE(dependency->object(), referencedObject);
    QCOMPARE(dependency->property().name(), "labelText");
    QCOMPARE(dependency->isActive(), true);
    QCOMPARE(dependency->isBindingLoop(), false);
    QCOMPARE(dependency->cachedValue(), QStringLiteral("Hello world!"));
    QCOMPARE(dependency->dependencies().size(), 0);

    delete rect;
}

void BindingInspectorTest::testQtQuickProvider_data()
{
    QTest::addColumn<QByteArray>("code");
    QTest::addColumn<QString>("objName");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QStringList>("expectedDependencies");

    QTest::newRow("implicitWidth_determines_width_noLoop")
        << QByteArray("import QtQuick 2.0\n"
                    "Rectangle {\n"
                        "id: rect\n"
                        "objectName: 'rect'\n"
                        "implicitWidth: 20\n"
                        "Text { objectName: 'text'; width: parent.width }\n"
                    "}") << "rect" << "width" << QStringList { "rect.implicitWidth" };

    QTest::newRow("fill_determines_width")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    id: rect\n"
                      "    Item {\n"
                      "        objectName: 'item'\n"
                      "        anchors.fill: parent\n"
                      "    }\n"
                      "}\n"
        ) << "item" << "width" << QStringList {"rect.width", "leftMargin"};


    QTest::newRow("left_and_right_determine_width")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    id: rect\n"
                      "    Item {\n"
                      "        id: item\n"
                      "        objectName: 'item'\n"
                      "        anchors.left: parent.left\n"
                      "        anchors.right: parent.right\n"
                      "    }\n"
                      "}\n"
        ) << "item" << "width" << QStringList {"left", "right"};


    QTest::newRow("y_and_height_determine_bottom")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    id: rect\n"
                      "    Item {\n"
                      "        id: item\n"
                      "        objectName: 'item'\n"
                      "        y: 50\n"
                      "        height: 100\n"
                      "    }\n"
                      "}\n"
        ) << "item" << "bottom" << QStringList {"item.y", "item.height"};
}

void BindingInspectorTest::testQtQuickProvider()
{
    QFETCH(QByteArray, code);
    QFETCH(QString, objName);
    QFETCH(QString, propertyName);
    QFETCH(QStringList, expectedDependencies);

    QQmlEngine engine;
    QQmlComponent c(&engine);
    c.setData(code, QUrl());
    QObject *rect = c.create();
    QTest::qWait(10);
    QVERIFY(rect);

    QObject *obj = rect->objectName() == objName ? rect : rect->findChild<QObject *>(objName);
    QVERIFY(obj);

    BindingNode parentNode { obj, obj->metaObject()->indexOfProperty(qPrintable(propertyName)) };
    QuickImplicitBindingDependencyProvider provider;
    auto bindingNodes = provider.findDependenciesFor(&parentNode);

    for (QString depName : expectedDependencies) {
        bool found = false;
        for (auto &&bindingNode: bindingNodes) {
            if (bindingNode->canonicalName() == depName) {
                found = true;
                break;
            }
        }
        if (!found) {
            qDebug() << "Dependency" << depName << "not found. Actual dependencies:";
            for (auto &&bindingNode: bindingNodes) {
                qDebug() << "*" << (bindingNode->canonicalName());
            }
        }
        QVERIFY(found);
    }

    delete rect;
}

void BindingInspectorTest::testModel()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 2 },
        { qintptr(&obj2), &obj2, MockObject::staticMetaObject.indexOfProperty("b"), 3 },
        { 2,              &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 4 },
        { 2,              &obj2, MockObject::staticMetaObject.indexOfProperty("b"), 5 },
        { 5,              &obj2, MockObject::staticMetaObject.indexOfProperty("a"), 6 },
        { 6,              &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 7 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 2);
    QModelIndex node2Index = bindingModel.index(1, 0, QModelIndex());
    QVERIFY(node2Index.isValid());
    QCOMPARE(node2Index.data().toString(), "c");
    QCOMPARE(node2Index.sibling(1, 1).data().toChar(), 'x');
    QCOMPARE(node2Index.sibling(1, 4).data().toString(), "3");
    QCOMPARE(bindingModel.rowCount(node2Index), 2);

    QModelIndex node4Index = bindingModel.index(0, 0, node2Index);
    QVERIFY(node4Index.isValid());
    QCOMPARE(node4Index.data().toString(), "b");
    QCOMPARE(node4Index.sibling(0, 1).data().toBool(), true);
    QCOMPARE(node4Index.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(node4Index), 0);

    QModelIndex node5Index = bindingModel.index(1, 0, node2Index);
    QVERIFY(node5Index.isValid());
    QCOMPARE(node5Index.data().toString(), "b");
    QCOMPARE(node5Index.sibling(1, 1).data().toBool(), false);
    QCOMPARE(node5Index.sibling(1, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(node5Index), 1);

    QModelIndex node6Index = bindingModel.index(0, 0, node5Index);
    QVERIFY(node6Index.isValid());
    QCOMPARE(node6Index.data().toString(), "a");
    QCOMPARE(node6Index.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(node6Index.sibling(0, 4).data().toString(), "1");

    QModelIndex node7Index = bindingModel.index(0, 0, node6Index);
    QVERIFY(node7Index.isValid());
    QCOMPARE(node7Index.data().toString(), "a");
    QCOMPARE(node7Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node7Index.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(node7Index), 0);
}

void BindingInspectorTest::testModelDataChanged()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 2 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 3 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("d"), 4 },
        { 2,              &obj1, MockObject::staticMetaObject.indexOfProperty("e"), 5 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex node1Index = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(node1Index.isValid());
    QCOMPARE(node1Index.data().toString(), "a");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(node1Index), 3);

    QModelIndex node4Index = bindingModel.index(2, 0, node1Index);
    QVERIFY(node4Index.isValid());
    QCOMPARE(node4Index.data().toString(), "d");
    QCOMPARE(node4Index.sibling(2, 1).data().toDouble(), 5.3);
    QCOMPARE(node4Index.sibling(2, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node4Index), 0);

    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    obj1.setD(3.1415926535897932);
    obj1.setA(12);

    dataChangedSpy.wait(500);
    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), node4Index.sibling(2, 1));
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), node4Index.sibling(2, 1));

    QCOMPARE(bindingModel.rowCount(node1Index), 3);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "2");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 12);

    QCOMPARE(node4Index.sibling(2, 1).data().toDouble(), 3.1415926535897932);
    QCOMPARE(node4Index.sibling(2, 4).data().toString(), "0");
}

void BindingInspectorTest::testModelAdditions()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 2 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex node1Index = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(node1Index.isValid());
    QCOMPARE(node1Index.data().toString(), "a");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "1");
    QCOMPARE(bindingModel.rowCount(node1Index), 1);

    QModelIndex node2Index = bindingModel.index(0, 0, node1Index);
    QVERIFY(node2Index.isValid());
    QCOMPARE(node2Index.data().toString(), "c");
    QCOMPARE(node2Index.sibling(0, 1).data().toChar(), 'x');
    QCOMPARE(node2Index.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(node2Index), 0);

    QSignalSpy rowAddedSpy(&bindingModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.push_back({ 2, &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 });
    provider->data.push_back({ 2, &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 3 });
    provider->data.push_back({ 3, &obj1, MockObject::staticMetaObject.indexOfProperty("d"), 4 });
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 1);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);
    QCOMPARE(rowAddedSpy.front().front().value<QModelIndex>(), node2Index);

    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), node1Index.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), node2Index.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), node2Index.sibling(0, 4));
    QCOMPARE(dataChangedSpy.at(2).at(0).value<QModelIndex>(), node1Index.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).value<QModelIndex>(), node1Index.sibling(0, 4));

    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 12);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), QStringLiteral("∞"));

    QCOMPARE(node2Index.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel.rowCount(node2Index), 2);

    QModelIndex node1aIndex = bindingModel.index(0, 0, node2Index);
    QVERIFY(node1aIndex.isValid());
    QCOMPARE(node1aIndex.data().toString(), "a");
    QCOMPARE(node1aIndex.sibling(0, 1).data().toInt(), 12);
    QCOMPARE(node1aIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel.rowCount(node1aIndex), 0);

    QModelIndex node3Index = bindingModel.index(1, 0, node2Index);
    QVERIFY(node3Index.isValid());
    QCOMPARE(node3Index.data().toString(), "b");
    QCOMPARE(node3Index.sibling(1, 1).data().toBool(), true);
    QCOMPARE(node3Index.sibling(1, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(node3Index), 1);

    QModelIndex node4Index = bindingModel.index(0, 0, node3Index);
    QVERIFY(node4Index.isValid());
    QCOMPARE(node4Index.data().toString(), "d");
    QCOMPARE(node4Index.sibling(0, 1).data().toDouble(), 5.3);
    QCOMPARE(node4Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node4Index), 0);
}

void BindingInspectorTest::testModelInsertions()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("e"), 2 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex node1Index = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(node1Index.isValid());
    QCOMPARE(node1Index.data().toString(), "a");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "1");
    QCOMPARE(bindingModel.rowCount(node1Index), 1);

    QModelIndex node2Index = bindingModel.index(0, 0, node1Index);
    QVERIFY(node2Index.isValid());
    QCOMPARE(node2Index.data().toString(), "e");
    QCOMPARE(node2Index.sibling(0, 1).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(node2Index.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(node2Index), 0);

    QSignalSpy rowAddedSpy(&bindingModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.emplace(provider->data.end(), qintptr(1), &obj1, MockObject::staticMetaObject.indexOfProperty("b"), qintptr(3));
    provider->data.emplace(provider->data.end(), qintptr(1), &obj1, MockObject::staticMetaObject.indexOfProperty("c"), qintptr(4));
    provider->data.emplace(provider->data.end(), qintptr(4), &obj2, MockObject::staticMetaObject.indexOfProperty("a"), qintptr(5));
    provider->data.emplace(provider->data.end(), qintptr(2), &obj2, MockObject::staticMetaObject.indexOfProperty("a"), qintptr(6));
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 2);
    QCOMPARE(rowAddedSpy.front().at(0).value<QModelIndex>(), node1Index);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);

    QCOMPARE(node1Index.sibling(0, 4).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel.rowCount(node1Index), 3);

    node2Index = bindingModel.index(2, 0, node1Index);
    QVERIFY(node2Index.isValid());
    QCOMPARE(node2Index.data().toString(), "e");
    QCOMPARE(node2Index.sibling(2, 1).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(node2Index.sibling(2, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(node2Index), 1);



    QModelIndex node3Index = bindingModel.index(0, 0, node1Index);
    QVERIFY(node3Index.isValid());
    QCOMPARE(node3Index.data().toString(), "b");
    QCOMPARE(node3Index.sibling(0, 1).data().toBool(), true);
    QCOMPARE(node3Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node3Index), 0);

    QModelIndex node4Index = bindingModel.index(1, 0, node1Index);
    QVERIFY(node4Index.isValid());
    QCOMPARE(node4Index.data().toString(), "c");
    QCOMPARE(node4Index.sibling(1, 1).data().toChar(), 'x');
    QCOMPARE(node4Index.sibling(1, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(node4Index), 1);

    QModelIndex node5Index = bindingModel.index(0, 0, node4Index);
    QVERIFY(node5Index.isValid());
    QCOMPARE(node5Index.data().toString(), "a");
    QCOMPARE(node5Index.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(node5Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node5Index), 0);


    QCOMPARE(rowAddedSpy.back().at(0).value<QModelIndex>(), node2Index);
    QCOMPARE(rowAddedSpy.back().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.back().at(2).toInt(), 0);

    QModelIndex node6Index = bindingModel.index(0, 0, node2Index);
    QVERIFY(node6Index.isValid());
    QCOMPARE(node6Index.data().toString(), "a");
    QCOMPARE(node6Index.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(node6Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node6Index), 0);


    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), node1Index.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), node2Index.sibling(2, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), node2Index.sibling(2, 4));
    QCOMPARE(dataChangedSpy.at(2).at(0).value<QModelIndex>(), node1Index.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).value<QModelIndex>(), node1Index.sibling(0, 4));
}

void BindingInspectorTest::testModelRemovalAtEnd()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 2 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 3 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("d"), 4 },
        { 4,              &obj1, MockObject::staticMetaObject.indexOfProperty("e"), 5 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex node1Index = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(node1Index.isValid());
    QCOMPARE(node1Index.data().toString(), "a");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(node1Index), 3);

    QSignalSpy rowRemovedSpy(&bindingModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.erase(provider->data.end() - 3, provider->data.end());
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 1);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 2);
    QCOMPARE(rowRemovedSpy.front().front().value<QModelIndex>(), node1Index);

    QCOMPARE(bindingModel.rowCount(node1Index), 1);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "1");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 12);

    QModelIndex node2Index = bindingModel.index(0, 0, node1Index);
    QVERIFY(node2Index.isValid());
    QCOMPARE(node2Index.data().toString(), "b");
    QCOMPARE(node2Index.sibling(0, 1).data().toBool(), true);
    QCOMPARE(node2Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node2Index), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), node1Index.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), node1Index.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), node1Index.sibling(0, 4));
}

void BindingInspectorTest::testModelRemovalInside()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { qintptr(&obj1), &obj1, MockObject::staticMetaObject.indexOfProperty("a"), 1 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("b"), 2 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("c"), 3 },
        { 1,              &obj1, MockObject::staticMetaObject.indexOfProperty("d"), 4 },
        { 2,              &obj1, MockObject::staticMetaObject.indexOfProperty("e"), 5 },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex node1Index = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(node1Index.isValid());
    QCOMPARE(node1Index.data().toString(), "a");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(node1Index), 3);

    QSignalSpy rowRemovedSpy(&bindingModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.erase(provider->data.begin() + 1, provider->data.begin() + 3);
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 1);
    QCOMPARE(rowRemovedSpy.front().front().value<QModelIndex>(), node1Index);

    QCOMPARE(bindingModel.rowCount(node1Index), 1);
    QCOMPARE(node1Index.sibling(0, 4).data().toString(), "1");
    QCOMPARE(node1Index.sibling(0, 1).data().toInt(), 12);

    QModelIndex node4Index = bindingModel.index(0, 0, node1Index);
    QVERIFY(node4Index.isValid());
    QCOMPARE(node4Index.data().toString(), "d");
    QCOMPARE(node4Index.sibling(0, 1).data().toDouble(), 5.3);
    QCOMPARE(node4Index.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(node4Index), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), node1Index.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), node1Index.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), node1Index.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), node1Index.sibling(0, 4));
}

// void BindingInspectorTest::testFoo()
// {
//     QByteArray code =
// "            import QtQuick 2.0\n"
// "            Column {\n"
// "                id: col"
// "                height: 100\n"
// "                Rectangle {\n"
// "                    width: col.width\n"
// "                    height: 10\n"
// "                    color: 'red'\n"
// "                }\n"
// "            }";
//
//
//     QQmlEngine engine;
//     QQmlComponent c(&engine);
//     c.setData(code, QUrl());
//     QObject *rect = c.create();
//     QTest::qWait(10);
//     QVERIFY(rect);
//     QObject *text = rect->findChildren<QQuickRectangle *>().front();
//
//     bindingModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspector.qmlBindingModel"));
//     QVERIFY(bindingModel);
//     ModelTest modelTest(bindingModel);
//
//     Probe::instance()->selectObject(text);
//     QCOMPARE(bindingModel->rowCount(), 1);
//     QModelIndex node2Index = bindingModel->index(0, 0, QModelIndex());
//
//     QCOMPARE(bindingModel->rowCount(node2Index), 2);
//     QModelIndex dependencyIndex = bindingModel->index(1, 0, node2Index);
//     QVERIFY(dependencyIndex.isValid());
//     QCOMPARE(bindingModel->rowCount(dependencyIndex), 1);
//     QModelIndex dependency2Index = bindingModel->index(0, 0, dependencyIndex);
//     QVERIFY(dependency2Index.isValid());
//     auto data = bindingModel->itemData(dependency2Index);
//     qDebug() << data;
//     QVERIFY(data.size() > 0);
//     QCOMPARE(data[0].toString(), QStringLiteral("col.implicitWidth"));
//
//     Probe::instance()->selectObject(0); //TODO: is this the correct way (seems to crash in an independent part of GammaRay without)
//     delete rect;
// }

void BindingInspectorTest::testIntegration()
{
    QByteArray code =
        "import QtQuick 2.0\n"
        "Rectangle {\n"
        "    id: a\n"
        "    objectName: 'a'\n"
        "    property string labelText: \"Hello world!\"\n"
        "    implicitWidth: t.width\n"
        "    Text { id: t; objectName: 'text'; text: labelText; property int foo: y; anchors.fill: parent }\n"
        "}";

//     QmlBindingModel::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QmlBindingProvider));
//     QmlBindingModel::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QuickImplicitBindingDependencyProvider));

    QQmlEngine engine;
    QQmlComponent c(&engine);
    c.setData(code, QUrl());
    QObject *rect = c.create();
    QTest::qWait(10);
    QVERIFY(rect);
    QObject *text = rect->findChildren<QQuickText *>().front();

//     QString referencedObjectName = "a";

//     QObject *referencedObject = rect->objectName() == referencedObjectName ? rect : rect->findChild<QObject *>(referencedObjectName);
//     QVERIFY(referencedObject);

    bindingModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspector.bindingModel"));
    QVERIFY(bindingModel);
    ModelTest modelTest(bindingModel);

    Probe::instance()->selectObject(text);
    QCOMPARE(bindingModel->rowCount(), 2);
    QModelIndex textBindingIndex = bindingModel->index(0, 0, QModelIndex());
    QVERIFY(textBindingIndex.isValid());
    QCOMPARE(textBindingIndex.data().toString(), "t.text");
    QCOMPARE(textBindingIndex.sibling(0, 1).data().toString(), "Hello world!");
    QCOMPARE(textBindingIndex.sibling(0, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(textBindingIndex), 1);

    QModelIndex labelTextIndex = bindingModel->index(0, 0, textBindingIndex);
    QVERIFY(labelTextIndex.isValid());
    QCOMPARE(labelTextIndex.data().toString(), "a.labelText");
    QCOMPARE(labelTextIndex.sibling(0, 1).data().toString(), QStringLiteral("Hello world!"));
    QCOMPARE(labelTextIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(labelTextIndex), 0);

    QModelIndex anchorsFillIndex = bindingModel->index(1, 0, QModelIndex());
    QVERIFY(anchorsFillIndex.isValid());
    QCOMPARE(anchorsFillIndex.data().toString(), "t.anchors.fill");
//     QCOMPARE(anchorsFillIndex.sibling(0, 1).data().(), "");
    QCOMPARE(anchorsFillIndex.sibling(1, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(anchorsFillIndex), 1);

    QModelIndex implicitWidthIndex = bindingModel->index(0, 0, anchorsFillIndex);
    QVERIFY(implicitWidthIndex.isValid());
    QCOMPARE(implicitWidthIndex.data().toString(), "a.implicitWidth");
//     QCOMPARE(implicitWidthIndex.sibling(0, 1).data().toString(), QStringLiteral(""));
    QCOMPARE(implicitWidthIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(implicitWidthIndex), 0);

    Probe::instance()->selectObject(0); //TODO: is this the correct way (seems to crash in an independent part of GammaRay without)
    delete rect;
}

QTEST_MAIN(BindingInspectorTest)

#include "bindinginspectortest.moc"
