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
        for (auto &&dataItem : data) {
            if (dataItem.object == obj) {
                auto binding = new BindingNode(obj, obj->metaObject()->indexOfProperty(dataItem.propertyName));
                nodes.push_back(std::unique_ptr<BindingNode>(binding));
            }
        }
        return nodes;
    }

    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) override
    {
        std::vector<std::unique_ptr<BindingNode>> nodes;
        for (auto &&dataItem : data) {
            auto propertyIndex = dataItem.depObject->metaObject()->indexOfProperty(dataItem.propertyName);
            if (dataItem.object == binding->object() && propertyIndex == binding->propertyIndex()) {
                auto depPropertyIndex = dataItem.depObject->metaObject()->indexOfProperty(dataItem.depPropertyName);
                auto dependency = new BindingNode(dataItem.depObject, depPropertyIndex, binding);
                nodes.push_back(std::unique_ptr<BindingNode>(dependency));
            }
        }
        return nodes;
    }

    QString canonicalNameFor(BindingNode *) override
    {
        return {};
    }

    bool canProvideBindingsFor(QObject *) override
    {
        return true;
    }

    struct NodeData {
        NodeData(QObject *obj, const char *propName, QObject *depObj, const char *depPropName) // required for std::vector::emplace()
            : object(obj)
            , propertyName(propName)
            , depObject(depObj)
            , depPropertyName(depPropName)
        {}

        QObject *object;
        const char *propertyName;
        QObject *depObject;
        const char *depPropertyName;
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
        : QObject(Q_NULLPTR)
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
        {&obj1, "a", &obj1, "e"},
        {&obj1, "c", &obj1, "b"},
        {&obj1, "c", &obj2, "b"},
        {&obj2, "b", &obj2, "a"},
        {&obj2, "a", &obj1, "a"},
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
    QCOMPARE(dependencies1.size(), 1);

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
    QCOMPARE(dependencies2.size(), 2);
    auto &&dependency2 = dependencies2.front();
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
        ) << "item" << "width" << QStringList {"rect.width", "anchors.leftMargin"};


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
        ) << "item" << "width" << QStringList {"item.anchors.left", "item.anchors.right"};


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

    QTest::newRow("childrenRect")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    objectName: 'rect'\n"
                      "    Text {\n"
                      "        id: t\n"
                      "        text: 'Hello World!'\n"
                      "    }\n"
                      "}\n"
        ) << "rect" << "childrenRect" << QStringList {"t.height"};
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
        {&obj1, "d", &obj1, "e"},
        {&obj1, "c", &obj1, "b"},
        {&obj1, "c", &obj2, "b"},
        {&obj2, "b", &obj2, "a"},
        {&obj2, "a", &obj1, "a"},
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 2);
    QModelIndex obj1cIndex = bindingModel.index(1, 0, QModelIndex());
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), "c");
    QCOMPARE(obj1cIndex.sibling(1, 1).data().toChar(), 'x');
    QCOMPARE(obj1cIndex.sibling(1, 4).data().toString(), "3");
    QCOMPARE(bindingModel.rowCount(obj1cIndex), 2);

    QModelIndex obj1bIndex = bindingModel.index(0, 0, obj1cIndex);
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), "b");
    QCOMPARE(obj1bIndex.sibling(0, 1).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(obj1bIndex), 0);

    QModelIndex obj2bIndex = bindingModel.index(1, 0, obj1cIndex);
    QVERIFY(obj2bIndex.isValid());
    QCOMPARE(obj2bIndex.data().toString(), "b");
    QCOMPARE(obj2bIndex.sibling(1, 1).data().toBool(), false);
    QCOMPARE(obj2bIndex.sibling(1, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(obj2bIndex), 1);

    QModelIndex obj2aIndex = bindingModel.index(0, 0, obj2bIndex);
    QVERIFY(obj2aIndex.isValid());
    QCOMPARE(obj2aIndex.data().toString(), "a");
    QCOMPARE(obj2aIndex.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(obj2aIndex.sibling(0, 4).data().toString(), "1");

    QModelIndex obj1aIndex = bindingModel.index(0, 0, obj2aIndex);
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 0);
}

void BindingInspectorTest::testModelDataChanged()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "b", &obj1, "e" },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 2);
    QModelIndex obj1aIndex = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 3);

    QModelIndex obj1dIndex = bindingModel.index(2, 0, obj1aIndex);
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), "d");
    QCOMPARE(obj1dIndex.sibling(2, 1).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(2, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj1dIndex), 0);

    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    obj1.setD(3.1415926535897932);
    obj1.setA(12);

    dataChangedSpy.wait(500);
    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), obj1dIndex.sibling(2, 1));
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), obj1dIndex.sibling(2, 1));

    QCOMPARE(bindingModel.rowCount(obj1aIndex), 3);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "2");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 12);

    QCOMPARE(obj1dIndex.sibling(2, 1).data().toDouble(), 3.1415926535897932);
    QCOMPARE(obj1dIndex.sibling(2, 4).data().toString(), "0");
}

void BindingInspectorTest::testModelAdditions()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { &obj1, "a", &obj1, "c" }
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex obj1aIndex = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "1");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 1);

    QModelIndex obj1cIndex = bindingModel.index(0, 0, obj1aIndex);
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), "c");
    QCOMPARE(obj1cIndex.sibling(0, 1).data().toChar(), 'x');
    QCOMPARE(obj1cIndex.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(obj1cIndex), 0);

    QSignalSpy rowAddedSpy(&bindingModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.push_back({ &obj1, "c", &obj1, "a" });
    provider->data.push_back({ &obj1, "c", &obj1, "b" });
    provider->data.push_back({ &obj1, "b", &obj1, "d" });
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 1);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);
    QCOMPARE(rowAddedSpy.front().front().value<QModelIndex>(), obj1cIndex);

    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), obj1cIndex.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), obj1cIndex.sibling(0, 4));
    QCOMPARE(dataChangedSpy.at(2).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 4));

    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 12);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));

    QCOMPARE(obj1cIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel.rowCount(obj1cIndex), 2);

    QModelIndex node1aIndex = bindingModel.index(0, 0, obj1cIndex);
    QVERIFY(node1aIndex.isValid());
    QCOMPARE(node1aIndex.data().toString(), "a");
    QCOMPARE(node1aIndex.sibling(0, 1).data().toInt(), 12);
    QCOMPARE(node1aIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel.rowCount(node1aIndex), 0);

    QModelIndex obj1bIndex = bindingModel.index(1, 0, obj1cIndex);
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), "b");
    QCOMPARE(obj1bIndex.sibling(1, 1).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(1, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(obj1bIndex), 1);

    QModelIndex obj1dIndex = bindingModel.index(0, 0, obj1bIndex);
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), "d");
    QCOMPARE(obj1dIndex.sibling(0, 1).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj1dIndex), 0);
}

void BindingInspectorTest::testModelInsertions()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };

    provider->data = {{
        { &obj1, "a", &obj1, "e" }
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 1);
    QModelIndex obj1aIndex = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "1");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 1);

    QModelIndex obj1eIndex = bindingModel.index(0, 0, obj1aIndex);
    QVERIFY(obj1eIndex.isValid());
    QCOMPARE(obj1eIndex.data().toString(), "e");
    QCOMPARE(obj1eIndex.sibling(0, 1).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(obj1eIndex.sibling(0, 4).data().toString(), "0");
    QCOMPARE(bindingModel.rowCount(obj1eIndex), 0);

    QSignalSpy rowAddedSpy(&bindingModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.emplace(provider->data.end(), &obj1, "a", &obj1, "b");
    provider->data.emplace(provider->data.end(), &obj1, "a", &obj1, "c");
    provider->data.emplace(provider->data.end(), &obj1, "c", &obj2, "a");
    provider->data.emplace(provider->data.end(), &obj1, "e", &obj2, "a");
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 2);
    QCOMPARE(rowAddedSpy.front().at(0).value<QModelIndex>(), obj1aIndex);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);

    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 3);

    obj1eIndex = bindingModel.index(2, 0, obj1aIndex);
    QVERIFY(obj1eIndex.isValid());
    QCOMPARE(obj1eIndex.data().toString(), "e");
    QCOMPARE(obj1eIndex.sibling(2, 1).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(obj1eIndex.sibling(2, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(obj1eIndex), 1);



    QModelIndex obj1bIndex = bindingModel.index(0, 0, obj1aIndex);
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), "b");
    QCOMPARE(obj1bIndex.sibling(0, 1).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj1bIndex), 0);

    QModelIndex obj1cIndex = bindingModel.index(1, 0, obj1aIndex);
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), "c");
    QCOMPARE(obj1cIndex.sibling(1, 1).data().toChar(), 'x');
    QCOMPARE(obj1cIndex.sibling(1, 4).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel.rowCount(obj1cIndex), 1);

    QModelIndex obj2aIndex = bindingModel.index(0, 0, obj1cIndex);
    QVERIFY(obj2aIndex.isValid());
    QCOMPARE(obj2aIndex.data().toString(), "a");
    QCOMPARE(obj2aIndex.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(obj2aIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj2aIndex), 0);


    QCOMPARE(rowAddedSpy.back().at(0).value<QModelIndex>(), obj1eIndex);
    QCOMPARE(rowAddedSpy.back().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.back().at(2).toInt(), 0);

    QModelIndex obj2aIndex2 = bindingModel.index(0, 0, obj1eIndex);
    QVERIFY(obj2aIndex2.isValid());
    QCOMPARE(obj2aIndex2.data().toString(), "a");
    QCOMPARE(obj2aIndex2.sibling(0, 1).data().toInt(), 35);
    QCOMPARE(obj2aIndex2.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj2aIndex2), 0);


    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), obj1eIndex.sibling(2, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), obj1eIndex.sibling(2, 4));
    QCOMPARE(dataChangedSpy.at(2).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 4));
}

void BindingInspectorTest::testModelRemovalAtEnd()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "d", &obj1, "e" },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 2);
    QModelIndex obj1aIndex = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 3);

    QSignalSpy rowRemovedSpy(&bindingModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.erase(provider->data.end() - 3, provider->data.end());
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 1);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 2);
    QCOMPARE(rowRemovedSpy.front().front().value<QModelIndex>(), obj1aIndex);

    QCOMPARE(bindingModel.rowCount(obj1aIndex), 1);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "1");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 12);

    QModelIndex obj1bIndex = bindingModel.index(0, 0, obj1aIndex);
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), "b");
    QCOMPARE(obj1bIndex.sibling(0, 1).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj1bIndex), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 4));
}

void BindingInspectorTest::testModelRemovalInside()
{
    QmlBindingModel bindingModel;
    ModelTest modelTest(&bindingModel);
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "b", &obj1, "e" },
    }};

    bindingModel.setObject(&obj1);
    QCOMPARE(bindingModel.rowCount(QModelIndex()), 2);
    QModelIndex obj1aIndex = bindingModel.index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), "a");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "2");
    QCOMPARE(bindingModel.rowCount(obj1aIndex), 3);

    QSignalSpy rowRemovedSpy(&bindingModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    QSignalSpy dataChangedSpy(&bindingModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)));

    provider->data.erase(provider->data.begin(), provider->data.begin() + 2);
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().front().value<QModelIndex>(), obj1aIndex);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 1);

    QCOMPARE(bindingModel.rowCount(obj1aIndex), 1);
    QCOMPARE(obj1aIndex.sibling(0, 4).data().toString(), "1");
    QCOMPARE(obj1aIndex.sibling(0, 1).data().toInt(), 12);

    QModelIndex obj1dIndex = bindingModel.index(0, 0, obj1aIndex);
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), "d");
    QCOMPARE(obj1dIndex.sibling(0, 1).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel.rowCount(obj1dIndex), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 1)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 1));
    QCOMPARE(dataChangedSpy.at(1).at(0).value<QModelIndex>(), obj1aIndex.sibling(0, 4)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).value<QModelIndex>(), obj1aIndex.sibling(0, 4));
}

void BindingInspectorTest::testIntegration()
{
    provider->data = {};

    QByteArray code =
        "import QtQuick 2.0\n"
        "Rectangle {\n"
        "    id: a\n"
        "    objectName: 'a'\n"
        "    property string labelText: \"Hello world!\"\n"
        "    implicitWidth: childrenRect.width\n"
        "    height: 200\n"
        "    Text {\n"
        "       id: t\n"
        "       objectName: 'text'\n"
        "       text: labelText\n"
        "       property int foo: width\n"
        "       anchors { left: parent.left; right: parent.right; bottom: parent.bottom; verticalCenter: parent.verticalCenter }\n"
        "    }\n"
        "}";

    QQmlEngine engine;
    QQmlComponent c(&engine);
    c.setData(code, QUrl());
    QObject *rect = c.create();
    QTest::qWait(10);
    QVERIFY(rect);
    QObject *text = rect->findChildren<QQuickText *>().front();

    bindingModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspector.bindingModel"));
    QVERIFY(bindingModel);
    ModelTest modelTest(bindingModel);

    Probe::instance()->selectObject(text);
    QCOMPARE(bindingModel->rowCount(), 6);
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

    QModelIndex fooIndex = bindingModel->index(1, 0, QModelIndex());
    QVERIFY(fooIndex.isValid());
    QCOMPARE(fooIndex.data().toString(), "t.foo");
    QCOMPARE(fooIndex.sibling(1, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(fooIndex), 1);

    QModelIndex tWidthIndex = bindingModel->index(0, 0, fooIndex);
    QVERIFY(tWidthIndex.isValid());
    QCOMPARE(tWidthIndex.data().toString(), "t.width");
    QCOMPARE(tWidthIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(tWidthIndex), 2);

    QModelIndex tAnchorsRightIndex = bindingModel->index(1, 0, tWidthIndex);
    QVERIFY(tAnchorsRightIndex.isValid());
    QCOMPARE(tAnchorsRightIndex.data().toString(), "t.anchors.right");
    QCOMPARE(tAnchorsRightIndex.sibling(1, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(tAnchorsRightIndex), 2); // is `parent` and `parent.right`

    QModelIndex aRightIndex;
    QModelIndex aRightIndex0 = bindingModel->index(0, 0, tAnchorsRightIndex); // It's more or less random which row contains
    QModelIndex aRightIndex1 = bindingModel->index(1, 0, tAnchorsRightIndex); // the correct property.
    QVERIFY((aRightIndex0.data().toString() == "a.right" && aRightIndex1.data().toString() == "t.parent")
            || (aRightIndex1.data().toString() == "a.right" && aRightIndex0.data().toString() == "t.parent"));
    if (aRightIndex0.data().toString() == "a.right") {
        aRightIndex = aRightIndex0;
        QCOMPARE(aRightIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
        QCOMPARE(bindingModel->rowCount(aRightIndex), 2);
    } else {
        aRightIndex = aRightIndex1;
        QCOMPARE(aRightIndex.sibling(1, 4).data().toString(), QStringLiteral("∞"));
        QCOMPARE(bindingModel->rowCount(aRightIndex), 2);
    }
    QVERIFY(aRightIndex.isValid());

    QModelIndex aWidthIndex = bindingModel->index(1, 0, aRightIndex);
    QVERIFY(aWidthIndex.isValid());
    QCOMPARE(aWidthIndex.data().toString(), "a.width");
    QCOMPARE(aWidthIndex.sibling(1, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(aWidthIndex), 1);

    QModelIndex aImplicitWidthIndex = bindingModel->index(0, 0, aWidthIndex);
    QVERIFY(aImplicitWidthIndex.isValid());
    QCOMPARE(aImplicitWidthIndex.data().toString(), "a.implicitWidth");
    QCOMPARE(aImplicitWidthIndex.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(aImplicitWidthIndex), 1);

    QModelIndex aChildrenRect = bindingModel->index(0, 0, aImplicitWidthIndex);
    QVERIFY(aChildrenRect.isValid());
    QCOMPARE(aChildrenRect.data().toString(), "a.childrenRect");
    QCOMPARE(aChildrenRect.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(aChildrenRect), 2);

    QModelIndex tWidthIndex2 = bindingModel->index(0, 0, aChildrenRect);
    QVERIFY(tWidthIndex2.isValid());
    QCOMPARE(tWidthIndex2.data().toString(), "t.width");
    QCOMPARE(tWidthIndex2.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(tWidthIndex2), 0);

    Probe::instance()->selectObject(rect);
    QCOMPARE(bindingModel->rowCount(), 1);
    QModelIndex aImplicitWidthIndex2 = bindingModel->index(0, 0, QModelIndex());
    QVERIFY(aImplicitWidthIndex2.isValid());
    QCOMPARE(aImplicitWidthIndex2.data().toString(), "a.implicitWidth");
    QCOMPARE(aImplicitWidthIndex2.sibling(0, 1).data().toDouble(), 0.0);
    QCOMPARE(aImplicitWidthIndex2.sibling(0, 4).data().toString(), QStringLiteral("∞"));
    QCOMPARE(bindingModel->rowCount(aImplicitWidthIndex2), 1);

    Probe::instance()->selectObject(text);
    QCOMPARE(bindingModel->rowCount(), 6);
    QModelIndex tAnchorsVerticalCenterIndex = bindingModel->index(5, 0, QModelIndex());
    QVERIFY(tAnchorsVerticalCenterIndex.isValid());
    QCOMPARE(tAnchorsVerticalCenterIndex.data().toString(), "t.anchors.verticalCenter");
    QCOMPARE(tAnchorsVerticalCenterIndex.sibling(5, 4).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(tAnchorsVerticalCenterIndex), 2); // is `parent` and `parent.verticalCenter`

    QModelIndex aVerticalCenterIndex;
    QModelIndex aVerticalCenterIndex0 = bindingModel->index(0, 0, tAnchorsVerticalCenterIndex); // It's more or less random which row contains
    QModelIndex aVerticalCenterIndex1 = bindingModel->index(1, 0, tAnchorsVerticalCenterIndex); // the correct property.
    QVERIFY((aVerticalCenterIndex0.data().toString() == "a.verticalCenter" && aVerticalCenterIndex1.data().toString() == "t.parent")
            || (aVerticalCenterIndex1.data().toString() == "a.verticalCenter" && aVerticalCenterIndex0.data().toString() == "t.parent"));
    if (aVerticalCenterIndex0.data().toString() == "a.verticalCenter") {
        aVerticalCenterIndex = aVerticalCenterIndex0;
        QCOMPARE(aVerticalCenterIndex.sibling(0, 4).data().toString(), QStringLiteral("1"));
        QCOMPARE(bindingModel->rowCount(aVerticalCenterIndex), 2);
    } else {
        aVerticalCenterIndex = aVerticalCenterIndex1;
        QCOMPARE(aVerticalCenterIndex.sibling(1, 4).data().toString(), QStringLiteral("1"));
        QCOMPARE(bindingModel->rowCount(aVerticalCenterIndex), 2);
    }
    QVERIFY(aVerticalCenterIndex.isValid());

    QModelIndex aYIndex = bindingModel->index(0, 0, aVerticalCenterIndex);
    QVERIFY(aYIndex.isValid());
    QCOMPARE(aYIndex.data().toString(), "a.y");
    QCOMPARE(aYIndex.sibling(0, 1).data().toString(), QStringLiteral("0"));
    QCOMPARE(aYIndex.sibling(0, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(aYIndex), 0);

    QModelIndex aHeightIndex = bindingModel->index(1, 0, aVerticalCenterIndex);
    QVERIFY(aHeightIndex.isValid());
    QCOMPARE(aHeightIndex.data().toString(), "a.height");
    QCOMPARE(aHeightIndex.sibling(1, 1).data().toString(), QStringLiteral("200"));
    QCOMPARE(aHeightIndex.sibling(1, 4).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(aHeightIndex), 0);

    Probe::instance()->selectObject(Q_NULLPTR); //TODO: is this the correct way (seems to crash in an independent part of GammaRay without)
    delete rect;
}

QTEST_MAIN(BindingInspectorTest)

#include "bindinginspectortest.moc"
