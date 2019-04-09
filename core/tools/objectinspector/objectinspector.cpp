/*
  objectinspector.cpp

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

#include "objectinspector.h"
#include "propertycontroller.h"
#include "methodsextension.h"
#include "classinfoextension.h"
#include "enumsextension.h"
#include "propertiesextension.h"
#include "connectionsextension.h"
#include "applicationattributeextension.h"
#include "bindingextension.h"
#include "stacktraceextension.h"

#include "inboundconnectionsmodel.h"
#include "outboundconnectionsmodel.h"
#include "objectdataprovider.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <core/bindingaggregator.h>
#include <core/problemcollector.h>
#include <core/util.h>
#include <remote/serverproxymodel.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QMetaMethod>

#include <QMutexLocker>
#include <QThread>

using namespace GammaRay;

ObjectInspector::ObjectInspector(Probe *probe, QObject *parent)
    : QObject(parent)
{
    registerPCExtensions();

    m_propertyController = new PropertyController(QStringLiteral(
                                                      "com.kdab.GammaRay.ObjectInspector"), this);

    auto proxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    proxy->setSourceModel(probe->objectTreeModel());
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.ObjectInspectorTree"), proxy);

    m_selectionModel = ObjectBroker::selectionModel(proxy);

    connect(m_selectionModel,
            &QItemSelectionModel::selectionChanged,
            this, &ObjectInspector::objectSelectionChanged);

    connect(probe, &Probe::objectSelected,
            this, &ObjectInspector::objectSelected);

    ProblemCollector::registerProblemChecker("com.kdab.GammaRay.ObjectInspector.BindingLoopScan",
                                             "Binding Loops",
                                             "Scans all QObjects for binding loops",
                                             &BindingAggregator::scanForBindingLoops);
    ProblemCollector::registerProblemChecker("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck",
                                             "Connection issues",
                                             "Scans all QObjects for direct cross-thread and duplicate connections",
                                             &ObjectInspector::scanForConnectionIssues);
    ProblemCollector::registerProblemChecker("com.kdab.GammaRay.ObjectInspector.ThreadAffinityCheck",
                                             "Threading issues",
                                             "Scans all QObjects for thread affinity issues",
                                             &ObjectInspector::scanForThreadAffinityIssues);
}

void ObjectInspector::objectSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        modelIndexSelected(QModelIndex());
    else
        modelIndexSelected(selection.first().topLeft());
}

void ObjectInspector::modelIndexSelected(const QModelIndex &index)
{
    if (index.isValid()) {
        QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
        m_propertyController->setObject(obj);
    } else {
        m_propertyController->setObject(nullptr);
    }
}

void ObjectInspector::objectSelected(QObject *object)
{
    const QAbstractItemModel *model = m_selectionModel->model();
    const QModelIndexList indexList
        = model->match(model->index(0, 0),
                       ObjectModel::ObjectRole,
                       QVariant::fromValue<QObject *>(object), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_selectionModel->select(
        index,
        QItemSelectionModel::Select | QItemSelectionModel::Clear
        |QItemSelectionModel::Rows | QItemSelectionModel::Current);
    // TODO: move this to the client side!
    // ui->objectTreeView->scrollTo(index);
    modelIndexSelected(index);
}

void ObjectInspector::registerPCExtensions()
{
    PropertyController::registerExtension<ClassInfoExtension>();
    PropertyController::registerExtension<MethodsExtension>();
    PropertyController::registerExtension<EnumsExtension>();
    PropertyController::registerExtension<PropertiesExtension>();
    PropertyController::registerExtension<ConnectionsExtension>();
    PropertyController::registerExtension<ApplicationAttributeExtension>();
    PropertyController::registerExtension<BindingExtension>();
    PropertyController::registerExtension<StackTraceExtension>();
}

QVector<QByteArray> GammaRay::ObjectInspectorFactory::selectableTypes() const
{
    return QVector<QByteArray>() << QObject::staticMetaObject.className();
}

void ObjectInspector::scanForConnectionIssues()
{
    const QVector<QObject*> &allObjects = Probe::instance()->allQObjects();

    QMutexLocker lock(Probe::objectLock());
    for (QObject *obj : allObjects) {
        if (!Probe::instance()->isValidObject(obj))
            continue;

        auto reportProblem = [obj](const AbstractConnectionsModel::Connection &connection, const QString &descriptionTemplate, const QString &problemType, bool isOutbound) {
                QObject *sender = isOutbound ? obj : connection.endpoint.data();
                QObject *receiver = isOutbound ? connection.endpoint.data() : obj;
                if (!sender || !receiver) {
                    return;
                }

                QString signalName = sender->metaObject()->method(connection.signalIndex).name();
                QString slotName = connection.slotIndex < 0 ? QStringLiteral("<slot object>") : receiver->metaObject()->method(connection.slotIndex).name();
                QString senderName = Util::displayString(sender);
                QString receiverName = Util::displayString(receiver);
                Problem p;
                p.severity = Problem::Warning;
                p.description = descriptionTemplate.arg(receiverName, slotName, senderName, signalName);
                p.object = ObjectId(receiver);
//                 p.location = bindingNode->sourceLocation(); //TODO can we get source locations of connect-statements?
                p.problemId = QString("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck.%1:%2.%3-%4.%5")
                    .arg(problemType,
                            QString::number(reinterpret_cast<quintptr>(sender)),
                            QString::number(connection.signalIndex),
                            QString::number(reinterpret_cast<quintptr>(receiver)),
                            QString::number(connection.slotIndex));
                p.findingCategory = Problem::Scan;
                ProblemCollector::addProblem(p);
        };

        auto connections = InboundConnectionsModel::inboundConnectionsForObject(obj);
        for (auto it = connections.begin(); it != connections.end(); ++it) {
            auto &&connection = *it;

            if (AbstractConnectionsModel::isDuplicate(connections, connection)) {
                reportProblem(connection, QStringLiteral("The slot %1->%2 is connected to the signal %3->%4 multiple times."), QStringLiteral("Duplicate"), false);
            }
            if (AbstractConnectionsModel::isDirectCrossThreadConnection(obj, connection)) {
                reportProblem(connection, QStringLiteral("The connection of slot %1->%2 to the signal %3->%4 is a direct cross-thread connection."), QStringLiteral("CrossTread"), false);
            }
        }

        connections = OutboundConnectionsModel::outboundConnectionsForObject(obj);
        for (auto it = connections.begin(); it != connections.end(); ++it) {
            auto &&connection = *it;

            if (AbstractConnectionsModel::isDuplicate(connections, connection)) {
                reportProblem(connection, QStringLiteral("The slot %1->%2 is connected to the signal %3->%4 multiple times."), QStringLiteral("Duplicate"), true);
            }
            if (AbstractConnectionsModel::isDirectCrossThreadConnection(obj, connection)) {
                reportProblem(connection, QStringLiteral("The connection of slot %1->%2 to the signal %3->%4 is a direct cross-thread connection."), QStringLiteral("CrossTread"), true);
            }
        }
    }
}

void ObjectInspector::scanForThreadAffinityIssues()
{
    const auto probe = Probe::instance();
    const auto &objects = probe->allQObjects();

    QMutexLocker lock(Probe::objectLock());
    for (const auto object : objects) {
        if (!probe->isValidObject(object)) {
            continue;
        }

        const auto objectName = Util::displayString(object);
        if (object == object->thread()) {
            Problem problem;
            problem.severity = Problem::Warning;
            problem.description = QStringLiteral("The thread %1 has affinity with itself.").arg(objectName);
            problem.object = ObjectId(object);
            problem.locations.append(probe->objectCreationSourceLocation(object));
            problem.problemId = QStringLiteral("com.kdab.GammaRay.ObjectInspector.ThreadAffinityCheck.Self.%1")
                    .arg(QString::number(reinterpret_cast<quintptr>(object)));
            problem.findingCategory = Problem::Scan;
            ProblemCollector::addProblem(problem);
        }

        const auto parent = object->parent();
        if (parent == nullptr) {
            continue;
        }

        const auto parentName = Util::displayString(parent);
        if (object->thread() != parent->thread()) {
            Problem problem;
            problem.severity = Problem::Warning;
            problem.description = QStringLiteral("The object %1 doesn't have the same thread affinity as its parent %2.").arg(objectName, parentName);
            problem.object = ObjectId(object);
            problem.locations.append(probe->objectCreationSourceLocation(object));
            problem.problemId = QStringLiteral("com.kdab.GammaRay.ObjectInspector.ThreadAffinityCheck.%1:%2")
                    .arg(QString::number(reinterpret_cast<quintptr>(object)),
                         QString::number(reinterpret_cast<quintptr>(parent)));
            problem.findingCategory = Problem::Scan;
            ProblemCollector::addProblem(problem);
        }

        if (qobject_cast<QThread*>(parent) && object->thread() != object->parent()) {
            Problem problem;
            problem.severity = Problem::Warning;
            problem.description = QStringLiteral("The object %1 has thread %2 as parent, but doesn't have affinity with it.").arg(objectName, parentName);
            problem.object = ObjectId(object);
            problem.locations.append(probe->objectCreationSourceLocation(object));
            problem.problemId = QStringLiteral("com.kdab.GammaRay.ObjectInspector.ThreadAffinityCheck.Parent.%1")
                    .arg(QString::number(reinterpret_cast<quintptr>(object)),
                         QString::number(reinterpret_cast<quintptr>(parent)));
            problem.findingCategory = Problem::Scan;
            ProblemCollector::addProblem(problem);
        }
    }
}
