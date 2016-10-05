/*
  metaobjectbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "metaobjectbrowser.h"
#include "metaobjecttreemodel.h"
#include "probe.h"
#include "propertycontroller.h"

#include <common/objectbroker.h>
#include <common/metatypedeclarations.h>
#include <common/tools/metaobjectbrowser/qmetaobjectmodel.h>
#include <core/remote/serverproxymodel.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QDebug>
#include <QItemSelectionModel>

using namespace GammaRay;

MetaObjectBrowser::MetaObjectBrowser(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
    , m_propertyController(new PropertyController(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowser"), this))
{
    auto model = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    model->addRole(QMetaObjectModel::MetaObjectIssues);
    model->setSourceModel(Probe::instance()->metaObjectModel());
    m_model = model;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowserTreeModel"), m_model);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(m_model);

    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(objectSelected(QItemSelection)));

    m_propertyController->setMetaObject(0); // init

    connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)), this,
            SLOT(objectSelected(QObject*)));
    connect(probe->probe(), SIGNAL(nonQObjectSelected(void*,QString)), this,
            SLOT(objectSelected(void*,QString)));

    ObjectBroker::registerObject(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowser"), this);
}

void MetaObjectBrowser::rescanMetaTypes()
{
    qobject_cast<MetaObjectTreeModel*>(Probe::instance()->metaObjectModel())->scanMetaTypes();
}

void MetaObjectBrowser::objectSelected(const QItemSelection &selection)
{
    QModelIndex index;
    if (selection.size() == 1)
        index = selection.first().topLeft();

    if (index.isValid()) {
        const QMetaObject *metaObject
            = index.data(QMetaObjectModel::MetaObjectRole).value<const QMetaObject*>();
        m_propertyController->setMetaObject(metaObject);
    } else {
        m_propertyController->setMetaObject(0);
    }
}

void MetaObjectBrowser::objectSelected(QObject *obj)
{
    if (!obj)
        return;
    metaObjectSelected(obj->metaObject());
}

void MetaObjectBrowser::objectSelected(void *obj, const QString &typeName)
{
    if (typeName != QLatin1String("const QMetaObject*"))
        return;
    metaObjectSelected(static_cast<QMetaObject *>(obj));
}

void MetaObjectBrowser::metaObjectSelected(const QMetaObject *mo)
{
    if (!mo)
        return;
    const auto indexes = m_model->match(m_model->index(0, 0), QMetaObjectModel::MetaObjectRole,
                                        QVariant::fromValue(mo),
                                        Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexes.isEmpty()) {
        metaObjectSelected(mo->superClass());
        return;
    }
    ObjectBroker::selectionModel(m_model)->select(
        indexes.first(), QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
}

QVector<QByteArray> MetaObjectBrowserFactory::selectableTypes() const
{
    return QVector<QByteArray>() << QObject::staticMetaObject.className() << "QMetaObject";
}
