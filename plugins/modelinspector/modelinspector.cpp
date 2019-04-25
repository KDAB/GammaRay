/*
  modelinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelinspector.h"

#include "modelmodel.h"
#include "modelcellmodel.h"
#include "modelcontentproxymodel.h"
#include "selectionmodelmodel.h"

#include <core/remote/serverproxymodel.h>
#include <common/objectbroker.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QDebug>
#include <QItemSelectionModel>

using namespace GammaRay;

ModelInspector::ModelInspector(Probe *probe, QObject *parent)
    : ModelInspectorInterface(parent)
    , m_probe(probe)
    , m_modelModel(nullptr)
    , m_selectionModelsModel(new SelectionModelModel(this))
    , m_selectionModelsSelectionModel(nullptr)
    , m_modelContentSelectionModel(nullptr)
    , m_modelContentProxyModel(new ModelContentProxyModel(this))
{
    auto modelModelSource = new ModelModel(this);
    connect(probe, &Probe::objectCreated, modelModelSource, &ModelModel::objectAdded);
    connect(probe, &Probe::objectDestroyed, modelModelSource, &ModelModel::objectRemoved);

    auto modelModelProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    modelModelProxy->setSourceModel(modelModelSource);
    m_modelModel = modelModelProxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ModelModel"), m_modelModel);

    m_modelSelectionModel = ObjectBroker::selectionModel(m_modelModel);
    connect(m_modelSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &ModelInspector::modelSelected);
    connect(probe, &Probe::objectSelected, this, &ModelInspector::objectSelected);

    connect(probe, &Probe::objectCreated, m_selectionModelsModel, &SelectionModelModel::objectCreated);
    connect(probe, &Probe::objectDestroyed, m_selectionModelsModel, &SelectionModelModel::objectDestroyed);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SelectionModels"), m_selectionModelsModel);
    m_selectionModelsSelectionModel = ObjectBroker::selectionModel(m_selectionModelsModel);
    connect(m_selectionModelsSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &ModelInspector::selectionModelSelected);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ModelContent"), m_modelContentProxyModel);
    m_modelContentSelectionModel = ObjectBroker::selectionModel(m_modelContentProxyModel);
    connect(m_modelContentSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &ModelInspector::cellSelectionChanged);

    m_cellModel = new ModelCellModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ModelCellModel"), m_cellModel);

    if (m_probe->needsObjectDiscovery())
        connect(m_probe, &Probe::objectCreated, this, &ModelInspector::objectCreated);
}

void ModelInspector::modelSelected(const QItemSelection &selected)
{
    QModelIndex index;
    if (!selected.isEmpty())
        index = selected.first().topLeft();

    if (index.isValid()) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(obj);
        Q_ASSERT(model);
        m_selectionModelsModel->setModel(model);
        m_modelContentProxyModel->setSourceModel(model);
    } else {
        m_selectionModelsModel->setModel(nullptr);
        m_modelContentProxyModel->setSourceModel(nullptr);
    }

    // clear the cell info box
    setCurrentCellData(ModelCellData());
    m_cellModel->setModelIndex(QModelIndex());
    m_modelContentSelectionModel->clear();
}

void ModelInspector::objectSelected(QObject *object)
{
    if (auto model = qobject_cast<QAbstractItemModel*>(object)) {
        if (model == m_modelContentProxyModel->sourceModel())
            return;

        const auto indexList = m_modelModel->match(m_modelModel->index(0, 0),
                                  ObjectModel::ObjectRole,
                                  QVariant::fromValue<QObject*>(model), 1,
                                  Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
        if (indexList.isEmpty())
            return;

        const auto index = indexList.first();
        m_modelSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
    if (auto selModel = qobject_cast<QItemSelectionModel*>(object)) {
        if (!selModel->model())
            return;
        objectSelected(const_cast<QAbstractItemModel*>(selModel->model()));

        const auto indexList = m_selectionModelsModel->match(m_selectionModelsModel->index(0, 0),
                                  ObjectModel::ObjectRole,
                                  QVariant::fromValue<QObject*>(selModel), 1,
                                  Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
        if (indexList.isEmpty())
            return;

        const auto index = indexList.first();
        m_selectionModelsSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void ModelInspector::cellSelectionChanged(const QItemSelection &selection)
{
    QModelIndex index;
    if (!selection.isEmpty())
        index = selection.at(0).topLeft();

    const QModelIndex sourceIndex = m_modelContentProxyModel->mapToSource(index);
    m_cellModel->setModelIndex(sourceIndex);

    if (!sourceIndex.isValid()) {
        setCurrentCellData(ModelCellData());
        return;
    }

    ModelCellData cellData;
    cellData.row = sourceIndex.row();
    cellData.column = sourceIndex.column();
    cellData.internalId = QString::number(sourceIndex.internalId());
    cellData.internalPtr = Util::addressToString(sourceIndex.internalPointer());
    cellData.flags = sourceIndex.flags();
    setCurrentCellData(cellData);
}

void ModelInspector::objectCreated(QObject *object)
{
    if (!object)
        return;

    if (auto proxy = qobject_cast<QAbstractProxyModel *>(object))
        m_probe->discoverObject(proxy->sourceModel());
}

void ModelInspector::selectionModelSelected(const QItemSelection &selected)
{
    QModelIndex idx;
    if (!selected.isEmpty())
        idx = selected.at(0).topLeft();
    if (!idx.isValid()) {
        m_modelContentProxyModel->setSelectionModel(nullptr);
        return;
    }
    m_modelContentProxyModel->setSelectionModel(qobject_cast<QItemSelectionModel*>(idx.data(ObjectModel::ObjectRole).value<QObject*>()));
}
