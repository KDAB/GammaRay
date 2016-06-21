/*
  modelinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "modeltester.h"
#include "safetyfilterproxymodel.h"

#include "probeinterface.h"

#include "common/objectbroker.h"
#include "remote/remotemodelserver.h"
#include "remote/selectionmodelserver.h"

#include <3rdparty/kde/krecursivefilterproxymodel.h>
#include <QDebug>

using namespace GammaRay;

ModelInspector::ModelInspector(ProbeInterface *probe, QObject *parent)
    : ModelInspectorInterface(parent)
    , m_probe(probe)
    , m_modelModel(0)
    , m_modelContentServer(0)
    , m_modelContentSelectionModel(0)
    , m_safetyFilterProxyModel(0)
    , m_modelTester(0)
{
    auto modelModelSource = new ModelModel(this);
    connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
            modelModelSource, SLOT(objectAdded(QObject*)));
    connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
            modelModelSource, SLOT(objectRemoved(QObject*)));

    auto modelModelProxy = new KRecursiveFilterProxyModel(this);
    modelModelProxy->setSourceModel(modelModelSource);
    m_modelModel = modelModelProxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ModelModel"), m_modelModel);

    m_modelSelectionModel = ObjectBroker::selectionModel(m_modelModel);
    connect(m_modelSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(modelSelected(QItemSelection)));
    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
            SLOT(objectSelected(QObject*)));

    m_modelContentServer = new RemoteModelServer(QStringLiteral(
                                                     "com.kdab.GammaRay.ModelContent"), this);

    m_cellModel = new ModelCellModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ModelCellModel"), m_cellModel);
    selectionChanged(QModelIndex());

    m_modelTester = new ModelTester(this);
    connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
            m_modelTester, SLOT(objectAdded(QObject*)));

    if (m_probe->needsObjectDiscovery())
        connect(m_probe->probe(), SIGNAL(objectCreated(QObject*)), SLOT(objectCreated(QObject*)));
}

void ModelInspector::modelSelected(const QItemSelection &selected)
{
    if (m_modelContentSelectionModel && m_modelContentSelectionModel->model())
        ObjectBroker::unregisterSelectionModel(m_modelContentSelectionModel);
    delete m_modelContentSelectionModel;
    m_modelContentSelectionModel = 0;

    QModelIndex index;
    if (selected.size() >= 1)
        index = selected.first().topLeft();

    if (index.isValid()) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(obj);

        if (model->inherits("QQmlListModel")) {
            if (!m_safetyFilterProxyModel)
                m_safetyFilterProxyModel = new SafetyFilterProxyModel(this);
            m_safetyFilterProxyModel->setSourceModel(model);
            m_modelContentServer->setModel(m_safetyFilterProxyModel);
        } else {
            m_modelContentServer->setModel(model);
        }

        m_modelContentSelectionModel
            = new SelectionModelServer(QStringLiteral(
                                           "com.kdab.GammaRay.ModelContent.selection"),
                                       m_modelContentServer->model(), this);
        ObjectBroker::registerSelectionModel(m_modelContentSelectionModel);
        connect(m_modelContentSelectionModel,
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                SLOT(selectionChanged(QModelIndex)));
    } else {
        m_modelContentServer->setModel(0);
    }

    // clear the cell info box
    selectionChanged(QModelIndex());
}

void ModelInspector::objectSelected(QObject *object)
{
    QAbstractItemModel *selectedModel = qobject_cast<QAbstractItemModel *>(object);
    if (selectedModel) {
        const QModelIndexList indexList
            = m_modelModel->match(m_modelModel->index(0, 0),
                                  ObjectModel::ObjectRole,
                                  QVariant::fromValue<QObject *>(selectedModel), 1,
                                  Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
        if (indexList.isEmpty())
            return;

        const QModelIndex index = indexList.first();
        m_modelSelectionModel->select(index,
                                      QItemSelectionModel::ClearAndSelect
                                      | QItemSelectionModel::Rows);
    }
}

void ModelInspector::selectionChanged(const QModelIndex &index)
{
    m_cellModel->setModelIndex(index);

    emit cellSelected(index.row(), index.column(), QString::number(
                          index.internalId()), Util::addressToString(index.internalPointer()));
}

void ModelInspector::objectCreated(QObject *object)
{
    if (!object)
        return;

    if (auto proxy = qobject_cast<QAbstractProxyModel *>(object))
        m_probe->discoverObject(proxy->sourceModel());
}

QString ModelInspectorFactory::name() const
{
    return tr("Models");
}

QVector<QByteArray> ModelInspectorFactory::selectableTypes() const
{
    return QVector<QByteArray>() << QAbstractItemModel::staticMetaObject.className();
}
