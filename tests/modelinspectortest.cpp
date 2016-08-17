/*
  modelinspectortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <plugins/modelinspector/modelinspectorinterface.h>
#include <plugins/modelinspector/modelcontentproxymodel.h>

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <ui/clienttoolmanager.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/objectid.h>

#include <3rdparty/qt/modeltest.h>

#include <QtTest/qtest.h>

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QObject>

using namespace GammaRay;

class ModelInspectorTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
    }

    QModelIndex indexForName(QAbstractItemModel *model, const QString &name)
    {
        const auto matchResult = model->match(model->index(0, 0), Qt::DisplayRole, name, 1, Qt::MatchExactly | Qt::MatchRecursive);
        if (matchResult.size() < 1)
            return QModelIndex();
        const auto idx = matchResult.at(0);
        Q_ASSERT(idx.isValid());
        return idx;
    }

private slots:
    void testModelModel()
    {
        createProbe();

        auto targetModel = new QStandardItemModel;
        targetModel->setObjectName("targetModel");
        QTest::qWait(1); // trigger model inspector plugin loading

        auto modelModel = ObjectBroker::model("com.kdab.GammaRay.ModelModel");
        QVERIFY(modelModel);
        ModelTest modelModelTester(modelModel);
        QVERIFY(modelModel->rowCount() >= 1); // can contain the QEmptyModel instance too
        int topRowCount = modelModel->rowCount();

        auto targetModelIdx = indexForName(modelModel, QLatin1String("targetModel"));
        QVERIFY(targetModelIdx.isValid());
        QCOMPARE(targetModelIdx.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(targetModel));
        QCOMPARE(targetModelIdx.sibling(targetModelIdx.row(), 1).data().toString(), QLatin1String("QStandardItemModel"));
        QVERIFY(!modelModel->hasChildren(targetModelIdx));

        // proxy with source
        auto targetProxy = new QSortFilterProxyModel;
        targetProxy->setSourceModel(targetModel);
        QTest::qWait(1);

        targetModelIdx = indexForName(modelModel, QLatin1String("targetModel")); // re-lookup, due to model reset
        QVERIFY(targetModelIdx.isValid());
        QVERIFY(modelModel->hasChildren(targetModelIdx));
        QCOMPARE(modelModel->rowCount(), topRowCount);

        delete targetProxy;
        QTest::qWait(1);

        targetModelIdx = indexForName(modelModel, QLatin1String("targetModel")); // re-lookup, due to model reset
        QVERIFY(targetModelIdx.isValid());
        QVERIFY(!modelModel->hasChildren(targetModelIdx));
        QCOMPARE(modelModel->rowCount(), topRowCount);

        // proxy with source added/reset later
        targetProxy = new QSortFilterProxyModel;
        targetProxy->setObjectName("targetProxy");
        QTest::qWait(1);
        QCOMPARE(modelModel->rowCount(), topRowCount + 1);
        auto proxyIdx = indexForName(modelModel, "targetProxy");
        QVERIFY(proxyIdx.isValid());
        QVERIFY(!proxyIdx.parent().isValid());

        targetProxy->setSourceModel(targetModel);
        QCOMPARE(modelModel->rowCount(), topRowCount);
        proxyIdx = indexForName(modelModel, "targetProxy");
        QVERIFY(proxyIdx.isValid());
        QVERIFY(proxyIdx.parent().isValid());

        targetProxy->setSourceModel(Q_NULLPTR);
        QCOMPARE(modelModel->rowCount(), topRowCount + 1);
        proxyIdx = indexForName(modelModel, "targetProxy");
        QVERIFY(proxyIdx.isValid());
        QVERIFY(!proxyIdx.parent().isValid());

        delete targetProxy;
        QTest::qWait(1);
        QCOMPARE(modelModel->rowCount(), topRowCount);

        // 2 element proxy chain
        targetProxy = new QSortFilterProxyModel;
        targetProxy->setObjectName("targetProxy");
        QTest::qWait(1);
        auto targetProxy2 = new QSortFilterProxyModel(targetProxy);
        targetProxy2->setObjectName("targetProxy2");
        QTest::qWait(1);
        targetProxy2->setSourceModel(targetProxy);
        targetProxy->setSourceModel(targetModel);
        QCOMPARE(modelModel->rowCount(), topRowCount);
        proxyIdx = indexForName(modelModel, "targetProxy2");
        QVERIFY(proxyIdx.isValid());
        auto idx = proxyIdx.parent();
        QVERIFY(idx.isValid());
        QCOMPARE(modelModel->rowCount(idx), 1);
        QCOMPARE(idx.data().toString(), QLatin1String("targetProxy"));
        idx = idx.parent();
        QVERIFY(idx.isValid());
        QVERIFY(!idx.parent().isValid());
        QCOMPARE(modelModel->rowCount(idx), 1);
        QCOMPARE(idx.data().toString(), QLatin1String("targetModel"));

        delete targetProxy;
        QTest::qWait(1);
        QCOMPARE(modelModel->rowCount(), topRowCount);

        // QAIM removal
        delete targetModel;
        QTest::qWait(1);

        targetModelIdx = indexForName(modelModel, QLatin1String("targetModel"));
        QVERIFY(!targetModelIdx.isValid());
        QCOMPARE(modelModel->rowCount(), topRowCount - 1);
    }

    void testSelectionModels()
    {
        createProbe();

        auto targetModel = new QStringListModel;
        targetModel->setObjectName("targetModel");
        targetModel->setStringList(QStringList() << "item1" << "item2" << "item3");
        QTest::qWait(1); // trigger model inspector plugin loading

        auto modelModel = ObjectBroker::model("com.kdab.GammaRay.ModelModel");
        QVERIFY(modelModel);

        auto selectionModels = ObjectBroker::model("com.kdab.GammaRay.SelectionModels");
        QVERIFY(selectionModels);
        ModelTest selModelTester(selectionModels);
        QCOMPARE(selectionModels->rowCount(), 0);
        QSignalSpy resetSpy(selectionModels, SIGNAL(modelReset()));
        QVERIFY(resetSpy.isValid());

        auto targetSelModel = new QItemSelectionModel(targetModel);
        targetSelModel->setObjectName("targetSelModel");
        QTest::qWait(1);
        QCOMPARE(selectionModels->rowCount(), 0);

        auto modelSelModel = ObjectBroker::selectionModel(modelModel);
        QVERIFY(modelSelModel);
        auto idx = indexForName(modelModel, "targetModel");
        QVERIFY(idx.isValid());
        modelSelModel->select(idx, QItemSelectionModel::ClearAndSelect);
        QCOMPARE(selectionModels->rowCount(), 1);

        QSignalSpy dataChangeSpy(selectionModels, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
        QVERIFY(dataChangeSpy.isValid());
        QCOMPARE(selectionModels->index(0, 1).data().toInt(), 0);
        QCOMPARE(selectionModels->index(0, 2).data().toInt(), 0);
        QCOMPARE(selectionModels->index(0, 3).data().toInt(), 0);

        targetSelModel->select(targetModel->index(1, 0), QItemSelectionModel::ClearAndSelect);
        QCOMPARE(dataChangeSpy.size(), 1);
        QCOMPARE(selectionModels->index(0, 1).data().toInt(), 1);
        QCOMPARE(selectionModels->index(0, 2).data().toInt(), 1); // rows
        QCOMPARE(selectionModels->index(0, 3).data().toInt(), 0); // cols

        targetSelModel->clear();
        QCOMPARE(dataChangeSpy.size(), 2);

        delete targetSelModel;
        QTest::qWait(1);
        QCOMPARE(selectionModels->rowCount(), 0);

        delete targetModel;
        QTest::qWait(1);
        QVERIFY(resetSpy.isEmpty());
    }

    void testModelContent()
    {
        createProbe();

        auto targetModel = new QStandardItemModel;
        targetModel->setObjectName("targetModel");
        QTest::qWait(1); // trigger model inspector plugin loading

        auto modelModel = ObjectBroker::model("com.kdab.GammaRay.ModelModel");
        QVERIFY(modelModel);

        auto contentModel = ObjectBroker::model("com.kdab.GammaRay.ModelContent");
        QVERIFY(contentModel);
        ModelTest contentModelTester(contentModel);
        QCOMPARE(contentModel->rowCount(), 0);

        auto cellModel = ObjectBroker::model("com.kdab.GammaRay.ModelCellModel");
        QVERIFY(cellModel);
        ModelTest cellModelTester(cellModel);
        QCOMPARE(cellModel->rowCount(), 0);
        QSignalSpy cellContentResetSpy(cellModel, SIGNAL(modelReset()));
        QVERIFY(cellContentResetSpy.isValid());

        auto targetModelIdx = indexForName(modelModel, QLatin1String("targetModel"));
        QVERIFY(targetModelIdx.isValid());
        auto modelSelModel = ObjectBroker::selectionModel(modelModel);
        QVERIFY(modelSelModel);
        modelSelModel->select(targetModelIdx, QItemSelectionModel::ClearAndSelect);
        QCOMPARE(contentModel->rowCount(), 0);

        auto item = new QStandardItem("item0,0");
        item->setFlags(Qt::NoItemFlags); // should nevertheless be selectable for inspection
        targetModel->appendRow(item);
        QCOMPARE(contentModel->rowCount(), 1);
        QCOMPARE(contentModel->columnCount(), 1);
        auto idx = contentModel->index(0, 0);
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data().toString(), QLatin1String("item0,0"));
        QVERIFY(idx.flags() & Qt::ItemIsEnabled);
        QVERIFY(idx.flags() & Qt::ItemIsSelectable);
        QCOMPARE(idx.data(ModelContentProxyModel::DisabledRole).toBool(), true);

        auto cellSelModel = ObjectBroker::selectionModel(contentModel);
        QVERIFY(cellSelModel);
        qDebug() << "selecting" << cellSelModel->model();
        cellSelModel->select(idx, QItemSelectionModel::ClearAndSelect);
        QVERIFY(cellModel->rowCount() > 0);

        idx = indexForName(cellModel, QLatin1String("Qt::DisplayRole"));
        QVERIFY(idx.isValid());
        QCOMPARE(idx.sibling(idx.row(), 1).data().toString(), QLatin1String("item0,0"));

        auto iface = ObjectBroker::object<ModelInspectorInterface*>();
        QVERIFY(iface);
        auto cellData = iface->currentCellData();
        QCOMPARE(cellData.row, 0);
        QCOMPARE(cellData.column, 0);
        QCOMPARE(cellData.flags, Qt::NoItemFlags);

        cellSelModel->clear();
        QCOMPARE(cellModel->rowCount(), 0);
        QVERIFY(cellContentResetSpy.isEmpty());

        delete targetModel;
        QTest::qWait(1);
    }

    void testSelectionModelSelection()
    {
        createProbe();

        auto targetModel = new QStringListModel;
        targetModel->setObjectName("targetModel");
        targetModel->setStringList(QStringList() << "item1" << "item2" << "item3");
        QTest::qWait(1); // trigger model inspector plugin loading

        auto modelModel = ObjectBroker::model("com.kdab.GammaRay.ModelModel");
        QVERIFY(modelModel);

        auto targetSelModel = new QItemSelectionModel(targetModel);
        targetSelModel->setObjectName("targetSelModel");
        QTest::qWait(1);

        auto selectionModels = ObjectBroker::model("com.kdab.GammaRay.SelectionModels");
        QVERIFY(selectionModels);

        auto modelSelModel = ObjectBroker::selectionModel(modelModel);
        QVERIFY(modelSelModel);
        auto idx = indexForName(modelModel, "targetModel");
        QVERIFY(idx.isValid());
        modelSelModel->select(idx, QItemSelectionModel::ClearAndSelect);

        auto selSelModel = ObjectBroker::selectionModel(selectionModels);
        QVERIFY(selSelModel);
        idx = indexForName(selectionModels, "targetSelModel");
        QVERIFY(idx.isValid());
        selSelModel->select(idx, QItemSelectionModel::ClearAndSelect);

        auto contentModel = ObjectBroker::model("com.kdab.GammaRay.ModelContent");
        QVERIFY(contentModel);
        QCOMPARE(contentModel->rowCount(), targetModel->rowCount());
        for (int i = 0; i < targetModel->rowCount(); ++i)
            QVERIFY(contentModel->index(i, 0).data(ModelContentProxyModel::SelectedRole).isNull());

        QSignalSpy contentSpy(contentModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
        QVERIFY(contentSpy.isValid());

        targetSelModel->select(contentModel->index(1, 0), QItemSelectionModel::ClearAndSelect);
        QVERIFY(contentModel->index(0, 0).data(ModelContentProxyModel::SelectedRole).isNull());
        QVERIFY(contentModel->index(1, 0).data(ModelContentProxyModel::SelectedRole).toBool());
        QVERIFY(contentModel->index(2, 0).data(ModelContentProxyModel::SelectedRole).isNull());

        QVERIFY(contentSpy.size() > 0);

        delete targetSelModel;
        delete targetModel;
    }

    void testWidget()
    {
        createProbe();

        auto targetModel = new QStringListModel;
        targetModel->setObjectName("targetModel");
        targetModel->setStringList(QStringList() << "item1" << "item2" << "item3");
        QTest::qWait(1); // trigger model inspector plugin loading

        ClientToolManager mgr;
        mgr.requestAvailableTools();
        auto widget = mgr.widgetForId("gammaray_modelinspector");
        QVERIFY(widget);
        widget->show();

        auto views = widget->findChildren<QAbstractItemView*>();
        foreach (auto view, views) {
            QVERIFY(view->model());
        }
    }
};

QTEST_MAIN(ModelInspectorTest)

#include "modelinspectortest.moc"
