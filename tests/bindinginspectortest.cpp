/*
  bindinginspectortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "baseprobetest.h"
#include "testhelpers.h"

#include <config-gammaray.h>
#include <core/probe.h>
#include <core/propertycontroller.h>
#include <common/paths.h>
#include <common/objectbroker.h>
#include <probe/hooks.h>
#include <probe/probecreator.h>

#include <core/abstractbindingprovider.h>
#include <core/bindingaggregator.h>
#include <core/bindingnode.h>
#include <core/tools/objectinspector/bindingextension.h>
#include <core/tools/objectinspector/bindingmodel.h>
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

template<typename CompareFunc>
    std::vector<QModelIndex> getSortedChildren(const QModelIndex &index, const QAbstractItemModel *model, CompareFunc compare)
{
    std::vector<QModelIndex> childIndices;
    int rowCount = model->rowCount(index);
    childIndices.reserve(rowCount);
    for (int i = 0; i < rowCount; ++i) {
        childIndices.push_back(model->index(i, 0, index));
    }
    std::sort(childIndices.begin(), childIndices.end(), compare);
    return childIndices;
}

std::vector<QModelIndex> getSortedChildren(const QModelIndex &index, const QAbstractItemModel *model = nullptr)
{
    if (!model) {
        model = index.model();
    }
    return getSortedChildren(index, model, [](const QModelIndex &a, const QModelIndex &b) { return a.data().toString() < b.data().toString(); });
}

class MockBindingProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) const override
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

    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) const override
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

    bool canProvideBindingsFor(QObject *) const override
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
    MockObject(int a, bool b, char c, double d, const QString &e)
        : QObject(nullptr)
        , m_a(a)
        , m_b(b)
        , m_c(c)
        , m_d(d)
        , m_e(e)
    {
    }

    int a() const
    {
        return m_a;
    }

    bool b() const
    {
        return m_b;
    }

    char c() const
    {
        return m_c;
    }

    double d() const
    {
        return m_d;
    }

    const QString &e() const
    {
        return m_e;
    }

    void setA(int a)
    {
        m_a = a;
        emit aChanged();
    }

    void setB(bool b)
    {
        m_b = b;
        emit bChanged();
    }

    void setC(char c)
    {
        m_c = c;
        emit cChanged();
    }

    void setD(double d)
    {
        m_d = d;
        emit dChanged();
    }

    void setE(const QString &e)
    {
        m_e = e;
        emit eChanged();
    }

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

class BindingInspectorTest : public BaseProbeTest
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void testMockProvider();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    void testQmlBindingProvider_data();
    void testQmlBindingProvider();
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    void testQtQuickProvider_data();
    void testQtQuickProvider();
#endif
    void testModel();
    void testModelDataChanged();
    void testModelAdditions();
    void testModelInsertions();
    void testModelRemovalAtEnd();
    void testModelRemovalInside();
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    void testIntegration();
#endif

private:
    MockBindingProvider *provider;
    BindingExtension *bindingExtension;
    std::unique_ptr<ModelTest> modelTest;
    QAbstractItemModel *bindingModel;
};

void BindingInspectorTest::initTestCase()
{
    QQmlEngine engine; // Needed to initialize the Qml support plugin
    provider = new MockBindingProvider;
    BindingAggregator::registerBindingProvider(std::unique_ptr<MockBindingProvider>(provider));

    QTest::qWait(1);
    bindingExtension = ObjectBroker::object<BindingExtension*>("com.kdab.GammaRay.ObjectInspector.bindingsExtension");
    QVERIFY(bindingExtension);
    bindingModel = bindingExtension->model();
    QVERIFY(bindingModel);
    modelTest.reset(new ModelTest(bindingModel));
}

void BindingInspectorTest::init()
{
}

void BindingInspectorTest::cleanup()
{
    provider->data.clear();
}

void BindingInspectorTest::testMockProvider()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };
    obj2.setObjectName("obj2");

    provider->data = {{
        {&obj1, "a", &obj1, "e"},
        {&obj1, "c", &obj1, "b"},
        {&obj1, "c", &obj2, "b"},
        {&obj2, "b", &obj2, "a"},
        {&obj2, "a", &obj1, "a"},
    }};

    auto bindings1 = provider->findBindingsFor(&obj1);
    auto &&bindingNode1 = bindings1.at(0);

    QVERIFY(bindingNode1 != nullptr);
    QVERIFY(bindingNode1->parent() == nullptr);
    QCOMPARE(bindingNode1->object(), &obj1);
    QCOMPARE(bindingNode1->property().name(), "a");
    QCOMPARE(bindingNode1->isPartOfBindingLoop(), false);
    QCOMPARE(bindingNode1->cachedValue().toInt(), 53);

    auto dependencies1 = provider->findDependenciesFor(bindingNode1.get());
    QCOMPARE(dependencies1.size(), size_t(1));

    auto bindings2 = provider->findBindingsFor(&obj1);
    auto &&bindingNode2 = bindings2.back();

    QVERIFY(bindingNode2 != nullptr);
    QVERIFY(bindingNode2->parent() == nullptr);
    QCOMPARE(bindingNode2->object(), &obj1);
    QCOMPARE(bindingNode2->property().name(), "c");
    QCOMPARE(bindingNode2->isPartOfBindingLoop(), false);
    QCOMPARE(bindingNode2->cachedValue().toChar(), QChar('x'));

    auto dependencies2 = provider->findDependenciesFor(bindingNode2.get());
    QCOMPARE(dependencies2.size(), size_t(2));
    auto &&dependency2 = dependencies2.at(0);
    QCOMPARE(dependency2->parent(), bindingNode2.get());
    QCOMPARE(dependency2->object(), &obj1);
    QCOMPARE(dependency2->property().name(), "b");
    QCOMPARE(dependency2->isPartOfBindingLoop(), false);
    QCOMPARE(dependency2->cachedValue().toBool(), true);
    auto &&dependency3 = dependencies2.back();
    QCOMPARE(dependency3->parent(), bindingNode2.get());
    QCOMPARE(dependency3->object(), &obj2);
    QCOMPARE(dependency3->property().name(), "b");
    QCOMPARE(dependency3->isPartOfBindingLoop(), false);
    QCOMPARE(dependency3->cachedValue().toBool(), false);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
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
    QObject *text = rect->findChildren<QQuickText *>().at(0);

    QObject *referencedObject = rect->objectName() == referencedObjectName ? rect : rect->findChild<QObject *>(referencedObjectName);
    QVERIFY(referencedObject);

    QmlBindingProvider provider;
    auto bindingNodes = provider.findBindingsFor(text);
    auto &&bindingNode = bindingNodes.front();

    QVERIFY(bindingNode);
    QCOMPARE(bindingNode->object(), text);
    QCOMPARE(bindingNode->property().name(), "text");
    QCOMPARE(bindingNode->isPartOfBindingLoop(), false);
    QCOMPARE(bindingNode->cachedValue(), QStringLiteral("Hello world!"));

    auto dependencies = provider.findDependenciesFor(bindingNode.get());
    QCOMPARE(dependencies.size(), 1);
    const std::unique_ptr<BindingNode> &dependency = dependencies.front();
    QCOMPARE(dependency->object(), referencedObject);
    QCOMPARE(dependency->property().name(), "labelText");
    QCOMPARE(dependency->isPartOfBindingLoop(), false);
    QCOMPARE(dependency->cachedValue(), QStringLiteral("Hello world!"));
    QCOMPARE(dependency->dependencies().size(), 0);

    delete rect;
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
void BindingInspectorTest::testQtQuickProvider_data()
{
    QTest::addColumn<QByteArray>("code", nullptr);
    QTest::addColumn<QString>("objName", nullptr);
    QTest::addColumn<QString>("propertyName", nullptr);
    QTest::addColumn<QStringList>("expectedDependencies", nullptr);

    QTest::newRow("implicitWidth_determines_width_noLoop")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    id: rect\n"
                      "    objectName: 'rect'\n"
                      "    implicitWidth: 20\n"
                      "    Text { objectName: 'text'; width: parent.width }\n"
                      "}\n"
                      )
        << "rect" << "width" << QStringList { "rect.implicitWidth" };

    QTest::newRow("fill_determines_width")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    id: rect\n"
                      "    Item {\n"
                      "        objectName: 'item'\n"
                      "        anchors.fill: parent\n"
                      "    }\n"
                      "}\n"
                      )
        << "item" << "width" << QStringList {"rect.width", "anchors.leftMargin"};

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
                      )
        << "item" << "width" << QStringList {"item.anchors.left", "item.anchors.right"};

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
                      "}\n")
        << "item" << "bottom" << QStringList {"item.y", "item.height"};

    QTest::newRow("childrenRect")
        << QByteArray("import QtQuick 2.0\n"
                      "Rectangle {\n"
                      "    objectName: 'rect'\n"
                      "    Text {\n"
                      "        id: t\n"
                      "        text: 'Hello World!'\n"
                      "    }\n"
                      "}\n"
                      )
        << "rect" << "childrenRect" << QStringList {"t.height"};
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

    for (const QString &depName : expectedDependencies) {
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
#endif

void BindingInspectorTest::testModel()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };

    provider->data = {{
        {&obj1, "d", &obj1, "e"},
        {&obj1, "c", &obj1, "b"},
        {&obj1, "c", &obj2, "d"},
        {&obj2, "d", &obj2, "a"},
        {&obj2, "a", &obj1, "a"},
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 2);
    auto topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex obj1cIndex = topLevelIndices[0];
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), QStringLiteral("c"));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::ValueColumn).data().toChar(), QChar('x'));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("3"));
    QCOMPARE(bindingModel->rowCount(obj1cIndex), 2);

    auto obj1cChildIndices = getSortedChildren(obj1cIndex);
    QModelIndex obj1bIndex = obj1cChildIndices[0];
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), QStringLiteral("b"));
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::ValueColumn).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1bIndex), 0);

    QModelIndex obj2bIndex = obj1cChildIndices[1];
    QVERIFY(obj2bIndex.isValid());
    QCOMPARE(obj2bIndex.data().toString(), QStringLiteral("d"));
    QCOMPARE(obj2bIndex.sibling(obj2bIndex.row(), BindingModel::ValueColumn).data().toDouble(), 3.5);
    QCOMPARE(obj2bIndex.sibling(obj2bIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(obj2bIndex), 1);

    QModelIndex obj2aIndex = bindingModel->index(0, 0, obj2bIndex);
    QVERIFY(obj2aIndex.isValid());
    QCOMPARE(obj2aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj2aIndex.sibling(obj2aIndex.row(), BindingModel::ValueColumn).data().toInt(), 35);
    QCOMPARE(obj2aIndex.sibling(obj2aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj2aIndex), 1);

    QModelIndex obj1aIndex = bindingModel->index(0, 0, obj2aIndex);
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 0);
}

void BindingInspectorTest::testModelDataChanged()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "b", &obj1, "e" },
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 2);
    auto topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex obj1aIndex = topLevelIndices[0];
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 3);

    auto obj1aChildren = getSortedChildren(obj1aIndex);
    QModelIndex obj1dIndex = obj1aChildren[2];
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), QStringLiteral("d"));
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1dIndex), 0);

    QSignalSpy dataChangedSpy(bindingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(dataChangedSpy.isValid());

    obj1.setD(3.1415926535897932);
    obj1.setA(12);

    dataChangedSpy.wait(500);
    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(1).toModelIndex(), obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn));

    QCOMPARE(bindingModel->rowCount(obj1aIndex), 3);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 12);

    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn).data().toDouble(), 3.1415926535897932);
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
}

void BindingInspectorTest::testModelAdditions()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    provider->data = {{
        { &obj1, "a", &obj1, "c" }
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 1);
    QModelIndex obj1aIndex = bindingModel->index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 1);

    QModelIndex obj1cIndex = bindingModel->index(0, 0, obj1aIndex);
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), QStringLiteral("c"));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::ValueColumn).data().toChar(), QChar('x'));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1cIndex), 0);

    QSignalSpy rowAddedSpy(bindingModel, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy dataChangedSpy(bindingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(rowAddedSpy.isValid());
    QVERIFY(dataChangedSpy.isValid());

    provider->data.emplace_back( &obj1, "c", &obj1, "a" );
    provider->data.emplace_back( &obj1, "c", &obj1, "b" );
    provider->data.emplace_back( &obj1, "b", &obj1, "d" );
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 1);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);
    QCOMPARE(rowAddedSpy.front().front().toModelIndex(), obj1cIndex);

    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).toModelIndex(), obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn));
    QCOMPARE(dataChangedSpy.at(2).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn));

    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 12);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));

    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(obj1cIndex), 2);

    auto obj1cChildren = getSortedChildren(obj1cIndex);
    QModelIndex node1aIndex = obj1cChildren[0];
    QVERIFY(node1aIndex.isValid());
    QCOMPARE(node1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(node1aIndex.sibling(node1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 12);
    QCOMPARE(node1aIndex.sibling(node1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(node1aIndex), 0);

    QModelIndex obj1bIndex = obj1cChildren[1];
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), QStringLiteral("b"));
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::ValueColumn).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj1bIndex), 1);

    QModelIndex obj1dIndex = bindingModel->index(0, 0, obj1bIndex);
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), QStringLiteral("d"));
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1dIndex), 0);
}

void BindingInspectorTest::testModelInsertions()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };
    MockObject obj2 { 35, false, 'y', 3.5, "Bye, World" };

    provider->data = {{
        { &obj1, "a", &obj1, "e" }
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 1);
    QModelIndex obj1aIndex = bindingModel->index(0, 0, QModelIndex());
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 1);

    QModelIndex obj1eIndex = bindingModel->index(0, 0, obj1aIndex);
    QVERIFY(obj1eIndex.isValid());
    QCOMPARE(obj1eIndex.data().toString(), QStringLiteral("e"));
    QCOMPARE(obj1eIndex.sibling(obj1eIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(obj1eIndex.sibling(obj1eIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1eIndex), 0);

    QSignalSpy rowAddedSpy(bindingModel, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy dataChangedSpy(bindingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(rowAddedSpy.isValid());
    QVERIFY(dataChangedSpy.isValid());

    provider->data.emplace(provider->data.end(), &obj1, "a", &obj1, "b");
    provider->data.emplace(provider->data.end(), &obj1, "a", &obj1, "c");
    provider->data.emplace(provider->data.end(), &obj1, "c", &obj2, "a");
    provider->data.emplace(provider->data.end(), &obj1, "e", &obj2, "a");
    obj1.setA(12);

    rowAddedSpy.wait(500);
    QCOMPARE(rowAddedSpy.size(), 2);
    QCOMPARE(rowAddedSpy.front().at(0).toModelIndex(), obj1aIndex);
    QCOMPARE(rowAddedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.front().at(2).toInt(), 1);

    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 3);

    auto obj1aChildren = getSortedChildren(obj1aIndex);
    obj1eIndex = obj1aChildren[2];
    QVERIFY(obj1eIndex.isValid());
    QCOMPARE(obj1eIndex.data().toString(), QStringLiteral("e"));
    QCOMPARE(obj1eIndex.sibling(obj1eIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("Hello World"));
    QCOMPARE(obj1eIndex.sibling(obj1eIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj1eIndex), 1);

    QModelIndex obj1bIndex = obj1aChildren[0];
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), QStringLiteral("b"));
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::ValueColumn).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1bIndex), 0);

    QModelIndex obj1cIndex = obj1aChildren[1];
    QVERIFY(obj1cIndex.isValid());
    QCOMPARE(obj1cIndex.data().toString(), QStringLiteral("c"));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::ValueColumn).data().toChar(), QChar('x'));
    QCOMPARE(obj1cIndex.sibling(obj1cIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(obj1cIndex), 1);

    QModelIndex obj2aIndex = bindingModel->index(0, 0, obj1cIndex);
    QVERIFY(obj2aIndex.isValid());
    QCOMPARE(obj2aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj2aIndex.sibling(obj2aIndex.row(), BindingModel::ValueColumn).data().toInt(), 35);
    QCOMPARE(obj2aIndex.sibling(obj2aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj2aIndex), 0);

    QCOMPARE(rowAddedSpy.back().at(0).toModelIndex(), obj1eIndex);
    QCOMPARE(rowAddedSpy.back().at(1).toInt(), 0);
    QCOMPARE(rowAddedSpy.back().at(2).toInt(), 0);

    QModelIndex obj2aIndex2 = bindingModel->index(0, 0, obj1eIndex);
    QVERIFY(obj2aIndex2.isValid());
    QCOMPARE(obj2aIndex2.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj2aIndex2.sibling(obj2aIndex2.row(), BindingModel::ValueColumn).data().toInt(), 35);
    QCOMPARE(obj2aIndex2.sibling(obj2aIndex2.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj2aIndex2), 0);

    QCOMPARE(dataChangedSpy.size(), 3);
    QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), obj1eIndex.sibling(obj1eIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).toModelIndex(), obj1eIndex.sibling(obj1eIndex.row(), BindingModel::DepthColumn));
    QCOMPARE(dataChangedSpy.at(2).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(2).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn));
}

void BindingInspectorTest::testModelRemovalAtEnd()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "d", &obj1, "e" },
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 2);
    auto topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex obj1aIndex = topLevelIndices[0];
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 3);

    QSignalSpy rowRemovedSpy(bindingModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    QSignalSpy dataChangedSpy(bindingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(rowRemovedSpy.isValid());
    QVERIFY(dataChangedSpy.isValid());

    provider->data.erase(provider->data.end() - 3, provider->data.end());
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 1);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 2);
    QCOMPARE(rowRemovedSpy.front().front().toModelIndex(), obj1aIndex);

    QCOMPARE(bindingModel->rowCount(obj1aIndex), 1);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 12);

    QModelIndex obj1bIndex = bindingModel->index(0, 0, obj1aIndex);
    QVERIFY(obj1bIndex.isValid());
    QCOMPARE(obj1bIndex.data().toString(), QStringLiteral("b"));
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::ValueColumn).data().toBool(), true);
    QCOMPARE(obj1bIndex.sibling(obj1bIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1bIndex), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn));
}

void BindingInspectorTest::testModelRemovalInside()
{
    MockObject obj1 { 53, true, 'x', 5.3, "Hello World" };

    provider->data = {{
        { &obj1, "a", &obj1, "b" },
        { &obj1, "a", &obj1, "c" },
        { &obj1, "a", &obj1, "d" },
        { &obj1, "b", &obj1, "e" },
    }};

    bindingExtension->setQObject(&obj1);
    QCOMPARE(bindingModel->rowCount(QModelIndex()), 2);
    auto topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex obj1aIndex = topLevelIndices[0];
    QVERIFY(obj1aIndex.isValid());
    QCOMPARE(obj1aIndex.data().toString(), QStringLiteral("a"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 53);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(obj1aIndex), 3);

    QSignalSpy rowRemovedSpy(bindingModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    QSignalSpy dataChangedSpy(bindingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QVERIFY(rowRemovedSpy.isValid());
    QVERIFY(dataChangedSpy.isValid());

    provider->data.erase(provider->data.begin(), provider->data.begin() + 2);
    obj1.setA(12);

    rowRemovedSpy.wait(500);
    QCOMPARE(rowRemovedSpy.size(), 1);
    QCOMPARE(rowRemovedSpy.front().front().toModelIndex(), obj1aIndex);
    QCOMPARE(rowRemovedSpy.front().at(1).toInt(), 0);
    QCOMPARE(rowRemovedSpy.front().at(2).toInt(), 1);

    QCOMPARE(bindingModel->rowCount(obj1aIndex), 1);
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn).data().toInt(), 12);

    QModelIndex obj1dIndex = bindingModel->index(0, 0, obj1aIndex);
    QVERIFY(obj1dIndex.isValid());
    QCOMPARE(obj1dIndex.data().toString(), QStringLiteral("d"));
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::ValueColumn).data().toDouble(), 5.3);
    QCOMPARE(obj1dIndex.sibling(obj1dIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(obj1dIndex), 0);

    QCOMPARE(dataChangedSpy.size(), 2);
    QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn)); // Fair enough, we did change the value.
    QCOMPARE(dataChangedSpy.at(0).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::ValueColumn));
    QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn)); // depth changed
    QCOMPARE(dataChangedSpy.at(1).at(1).toModelIndex(), obj1aIndex.sibling(obj1aIndex.row(), BindingModel::DepthColumn));
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
void BindingInspectorTest::testIntegration()
{
    createProbe();
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
    QObject *text = rect->findChildren<QQuickText *>().at(0);

    QAbstractItemModel *bindingModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspector.bindingModel"));
    QVERIFY(bindingModel);
    ModelTest modelTest(bindingModel);

    Probe::instance()->selectObject(text);
    QCOMPARE(bindingModel->rowCount(), 6);
    auto topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex textBindingIndex = topLevelIndices[5];
    QVERIFY(textBindingIndex.isValid());
    QCOMPARE(textBindingIndex.data().toString(), QStringLiteral("t.text"));
    QCOMPARE(textBindingIndex.sibling(textBindingIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("Hello world!"));
    QCOMPARE(textBindingIndex.sibling(textBindingIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(textBindingIndex), 1);

    QModelIndex labelTextIndex = bindingModel->index(0, 0, textBindingIndex);
    QVERIFY(labelTextIndex.isValid());
    QCOMPARE(labelTextIndex.data().toString(), QStringLiteral("a.labelText"));
    QCOMPARE(labelTextIndex.sibling(labelTextIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("Hello world!"));
    QCOMPARE(labelTextIndex.sibling(labelTextIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(labelTextIndex), 0);

    QModelIndex fooIndex = topLevelIndices[4];
    QVERIFY(fooIndex.isValid());
    QCOMPARE(fooIndex.data().toString(), QStringLiteral("t.foo"));
    QCOMPARE(fooIndex.sibling(fooIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(fooIndex), 1);

    QModelIndex tWidthIndex = bindingModel->index(0, 0, fooIndex);
    QVERIFY(tWidthIndex.isValid());
    QCOMPARE(tWidthIndex.data().toString(), QStringLiteral("t.width"));
    QCOMPARE(tWidthIndex.sibling(tWidthIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(tWidthIndex), 2);

    auto tWidthChildren = getSortedChildren(tWidthIndex);
    QModelIndex tAnchorsRightIndex = tWidthChildren[1];
    QVERIFY(tAnchorsRightIndex.isValid());
    QCOMPARE(tAnchorsRightIndex.data().toString(), QStringLiteral("t.anchors.right"));
    QCOMPARE(tAnchorsRightIndex.sibling(tAnchorsRightIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(tAnchorsRightIndex), 2); // is `parent` and `parent.right`

    auto tAnchorsRightChildren = getSortedChildren(tAnchorsRightIndex);
    QModelIndex aRightIndex = tAnchorsRightChildren[0];
    QVERIFY(aRightIndex.isValid());
    QCOMPARE(aRightIndex.data().toString(), QStringLiteral("a.right"));
    QCOMPARE(aRightIndex.sibling(aRightIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(aRightIndex), 2);

    auto aRightChildren = getSortedChildren(aRightIndex);
    QModelIndex aWidthIndex = aRightChildren[0];
    QVERIFY(aWidthIndex.isValid());
    QCOMPARE(aWidthIndex.data().toString(), QStringLiteral("a.width"));
    QCOMPARE(aWidthIndex.sibling(aWidthIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(aWidthIndex), 1);

    QModelIndex aImplicitWidthIndex = bindingModel->index(0, 0, aWidthIndex);
    QVERIFY(aImplicitWidthIndex.isValid());
    QCOMPARE(aImplicitWidthIndex.data().toString(), QStringLiteral("a.implicitWidth"));
    QCOMPARE(aImplicitWidthIndex.sibling(aImplicitWidthIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(aImplicitWidthIndex), 1);

    QModelIndex aChildrenRectIndex = bindingModel->index(0, 0, aImplicitWidthIndex);
    QVERIFY(aChildrenRectIndex.isValid());
    QCOMPARE(aChildrenRectIndex.data().toString(), QStringLiteral("a.childrenRect"));
    QCOMPARE(aChildrenRectIndex.sibling(aChildrenRectIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(aChildrenRectIndex), 2);

    auto aChildrenRectChildren = getSortedChildren(aChildrenRectIndex);
    QModelIndex tWidthIndex2 = aChildrenRectChildren[1];
    QVERIFY(tWidthIndex2.isValid());
    QCOMPARE(tWidthIndex2.data().toString(), QStringLiteral("t.width"));
    QCOMPARE(tWidthIndex2.sibling(tWidthIndex2.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(tWidthIndex2), 0);

    Probe::instance()->selectObject(rect);
    QCOMPARE(bindingModel->rowCount(), 1);
    QModelIndex aImplicitWidthIndex2 = bindingModel->index(0, 0, QModelIndex());
    QVERIFY(aImplicitWidthIndex2.isValid());
    QCOMPARE(aImplicitWidthIndex2.data().toString(), QStringLiteral("a.implicitWidth"));
    QCOMPARE(aImplicitWidthIndex2.sibling(aImplicitWidthIndex2.row(), BindingModel::ValueColumn).data().toDouble(), 0.0);
    QCOMPARE(aImplicitWidthIndex2.sibling(aImplicitWidthIndex2.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("\u221E"));
    QCOMPARE(bindingModel->rowCount(aImplicitWidthIndex2), 1);

    Probe::instance()->selectObject(text);
    QCOMPARE(bindingModel->rowCount(), 6);
    topLevelIndices = getSortedChildren(QModelIndex(), bindingModel);
    QModelIndex tAnchorsVerticalCenterIndex = topLevelIndices[3];
    QVERIFY(tAnchorsVerticalCenterIndex.isValid());
    QCOMPARE(tAnchorsVerticalCenterIndex.data().toString(), QStringLiteral("t.anchors.verticalCenter"));
    QCOMPARE(tAnchorsVerticalCenterIndex.sibling(tAnchorsVerticalCenterIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("2"));
    QCOMPARE(bindingModel->rowCount(tAnchorsVerticalCenterIndex), 2); // is `parent` and `parent.verticalCenter`

    auto tAnchorsVerticalCenterChildren = getSortedChildren(tAnchorsVerticalCenterIndex);
    QModelIndex aVerticalCenterIndex = tAnchorsVerticalCenterChildren[0];
    QCOMPARE(aVerticalCenterIndex.data().toString(), QStringLiteral("a.verticalCenter"));
    QCOMPARE(aVerticalCenterIndex.sibling(aVerticalCenterIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("1"));
    QCOMPARE(bindingModel->rowCount(aVerticalCenterIndex), 2);
    QVERIFY(aVerticalCenterIndex.isValid());

    QModelIndex aYIndex = bindingModel->index(0, 0, aVerticalCenterIndex);
    QVERIFY(aYIndex.isValid());
    QCOMPARE(aYIndex.data().toString(), "a.y");
    QCOMPARE(aYIndex.sibling(aYIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(aYIndex.sibling(aYIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(aYIndex), 0);

    QModelIndex aHeightIndex = bindingModel->index(1, 0, aVerticalCenterIndex);
    QVERIFY(aHeightIndex.isValid());
    QCOMPARE(aHeightIndex.data().toString(), QStringLiteral("a.height"));
    QCOMPARE(aHeightIndex.sibling(aHeightIndex.row(), BindingModel::ValueColumn).data().toString(), QStringLiteral("200"));
    QCOMPARE(aHeightIndex.sibling(aHeightIndex.row(), BindingModel::DepthColumn).data().toString(), QStringLiteral("0"));
    QCOMPARE(bindingModel->rowCount(aHeightIndex), 0);

    delete rect;
    QCOMPARE(bindingModel->rowCount(), 0);
}
#endif

QTEST_MAIN(BindingInspectorTest)

#include "bindinginspectortest.moc"
