/*
  wlcompositorinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#include "wlcompositorinspector.h"

#include <QAbstractTableModel>
#include <QFile>
#include <QWaylandClient>

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <common/objectmodel.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/remote/serverproxymodel.h>
#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <wayland-server.h>

namespace GammaRay
{

class ResourcesModel : public QAbstractItemModel
{
    struct ClientListener {
        wl_listener l;
        ResourcesModel *m;
    };

public:
    struct Resource {
        wl_listener destroyListener;
        wl_resource *resource;
        ResourcesModel *model;
        QVector<Resource *> children;
        Resource *parent;
        int depth;

        static void destroyed(wl_listener *listener, void *)
        {
            Resource *res = reinterpret_cast<Resource *>(listener);
            res->model->removeResource(res->resource);
        }
        static Resource *fromWlResource(wl_resource *res)
        {
            wl_listener *listener = wl_resource_get_destroy_listener(res, destroyed);
            return reinterpret_cast<Resource *>(listener);
        }
    };

    ResourcesModel()
        : m_client(nullptr)
    {
        wl_list_init(&m_listener.l.link);
    }

    void setClient(QWaylandClient *client)
    {
        beginResetModel();
        clear();
        endResetModel();

        wl_list_remove(&m_listener.l.link);
        wl_list_init(&m_listener.l.link);

        m_client = client;
        if (client) {
            wl_client_add_resource_created_listener(client->client(), &m_listener.l);
            m_listener.m = this;
            m_listener.l.notify = [](wl_listener *listener, void *data) {
                wl_resource *resource = static_cast<wl_resource *>(data);
                ResourcesModel *model = reinterpret_cast<ClientListener *>(listener)->m;
                model->addResource(resource);
            };

            wl_client_for_each_resource(client->client(), [](wl_resource *res, void *ud) {
                ResourcesModel *model = static_cast<ResourcesModel *>(ud);
                model->addResource(res);
                return WL_ITERATOR_CONTINUE;
            }, this);
        }
    }

    QWaylandClient *client() const { return m_client; }

    ~ResourcesModel()
    {
        clear();
    }

    void clear()
    {
        foreach (Resource *res, m_resources) {
            destroy(res);
        }
        m_resources.clear();
    }

    void destroy(Resource *res)
    {
        foreach (Resource *child, res->children) {
            destroy(child);
        }

        wl_list_remove(&res->destroyListener.link);
        delete res;
    }

    QModelIndex index(Resource *res) const
    {
        if (res->parent) {
            return index(res->parent->children.indexOf(res), 0, index(res->parent));
        }
        return index(m_resources.indexOf(res), 0, QModelIndex());
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const override
    {
        Resource *parentres = exists(parent) ? static_cast<Resource *>(parent.internalPointer()) : nullptr;
        Resource *res = parentres ? parentres->children.at(row) : m_resources.at(row);
        return createIndex(row, column, res);
    }

    QModelIndex parent(const QModelIndex &idx) const override
    {
        Resource *res = static_cast<Resource *>(idx.internalPointer());
        if (exists(idx) && res->parent) {
            return index(res->parent);
        }
        return QModelIndex();
    }

    int row(Resource *res) const
    {
        if (res->parent) {
            return res->parent->children.indexOf(res);
        }
        return m_resources.indexOf(res);
    }

    void addResource(wl_resource *res)
    {
        wl_resource *parentResource = 0; // wl_resource_get_parent(res);
        Resource *parent = parentResource ? Resource::fromWlResource(parentResource) : nullptr;
        int count = parent ? parent->children.count() : m_resources.count();

        beginInsertRows(parent ? index(parent) : QModelIndex(), count, count);

        Resource *r = new Resource;
        r->parent = parent;
        r->resource = res;
        r->depth = parent ? parent->depth + 1 : 0;
        r->model = this;
        wl_resource_add_destroy_listener(res, &r->destroyListener);
        r->destroyListener.notify = Resource::destroyed;
        if (parent) {
            parent->children << r;
        } else {
            m_resources << r;
        }
        m_allResources << r;

        endInsertRows();
    }

    void removeResource(wl_resource *wlres)
    {
        Resource *r = Resource::fromWlResource(wlres);
        int idx = r->parent ? r->parent->children.indexOf(r) : m_resources.indexOf(r);

        beginRemoveRows(r->parent ? index(r->parent) : QModelIndex(), idx, idx);
        if (r->parent) {
            r->parent->children.removeAt(idx);
        } else {
            m_resources.removeAt(idx);
        }
        m_allResources.remove(r);
        wl_list_remove(&r->destroyListener.link);
        delete r;
        endRemoveRows();
    }

    bool exists(const QModelIndex &index) const
    {
        auto *p = static_cast<Resource *>(index.internalPointer());
        return !p || m_allResources.contains(p);
    }

    int rowCount(const QModelIndex &index) const override
    {
        if (!exists(index))
            return 0;
        Resource *res = static_cast<Resource *>(index.internalPointer());
        return res ? res->children.count() : m_resources.count();
    }

    int columnCount(const QModelIndex &) const override
    {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (!exists(index))
          return QVariant();

        const Resource *resource = static_cast<Resource *>(index.internalPointer());
        wl_resource *res = resource->resource;

        switch (role) {
            case Qt::DisplayRole:
                return QString("%1@%2").arg(wl_resource_get_class(res), QString::number(wl_resource_get_id(res)));
            case Qt::ToolTipRole:
                return tr("Version: %1").arg(QString::number(wl_resource_get_version(res)));
        }
        return QVariant();
    }

    QVector<Resource *> m_resources;
    QSet<Resource *> m_allResources;
    ClientListener m_listener;
    QWaylandClient *m_client;
};

class ClientsModel : public QAbstractTableModel
{
public:
    enum Columns {
        PidColumn,
        CommandColumn,
        EndColumn
    };

    explicit ClientsModel(ProbeInterface *probe, QObject *parent)
        : QAbstractTableModel(parent)
        , m_probe(probe)
    {
    }

    QWaylandClient *client(int index) const
    {
        return m_clients.at(index);
    }

    void addClient(QWaylandClient *client)
    {
        beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count());
        m_clients.append(client);
        endInsertRows();
    }

    void removeClient(QWaylandClient *client)
    {
        int index = -1;
        for (int i = 0; i < m_clients.count(); ++i) {
            if (m_clients.at(i) == client) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            return;
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_clients.removeAt(index);
        endRemoveRows();
    }

    int rowCount(const QModelIndex &) const override
    {
        return m_clients.count();
    }

    int columnCount(const QModelIndex &) const override
    {
        return EndColumn;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        Q_UNUSED(role)
        auto client = m_clients.at(index.row());

        switch (index.column()) {
            case PidColumn:
                return client->processId();
            case CommandColumn: {
                auto pid = client->processId();
                QByteArray path;
                QTextStream(&path) << "/proc/" << pid << "/cmdline";
                QFile file(path);
                if (!file.open(QIODevice::ReadOnly)) {
                    return QStringLiteral("Not available :/");
                }

                QByteArray data = file.readAll();
                data.replace('\0', ' ');
                return data;
            }
        }
        return QVariant();
    }

    QMap<int, QVariant> itemData(const QModelIndex &index) const override
    {
        QMap<int, QVariant> map;
        auto insertRole = [&](int role) {
            map[role] = data(index, role);
        };
        insertRole(Qt::DisplayRole);
        return map;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int) const override
    {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case PidColumn:
                    return QStringLiteral("pid");
                case CommandColumn:
                    return QStringLiteral("command");
            }
        }
        return QString::number(section + 1);
    }

    QVector<QWaylandClient *> m_clients;
    ProbeInterface *m_probe;
};

WlCompositorInspector::WlCompositorInspector(ProbeInterface* probe, QObject* parent)
                     : WlCompositorInterface(parent)
{
    qWarning()<<"init probe"<<probe->objectTreeModel()<<probe->probe();

    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QWaylandCompositor, QWaylandObject);

    m_clientsModel = new ClientsModel(probe, this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.WaylandCompositorClientsModel"), m_clientsModel);

    m_resourcesModel = new ResourcesModel;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.WaylandCompositorResourcesModel"), m_resourcesModel);

    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), this, SLOT(objectAdded(QObject*)));
}

WlCompositorInspector::~WlCompositorInspector()
{
}

struct ClientsListener {
    wl_listener listener;
    WlCompositorInspector *inspector;
};

void WlCompositorInspector::objectAdded(QObject *obj)
{
    if (auto *compositor = qobject_cast<QWaylandCompositor *>(obj)) {
        init(compositor);
    }
}

void WlCompositorInspector::init(QWaylandCompositor *compositor)
{
    qWarning()<<"found compositor"<<compositor;
    m_compositor = compositor;

    wl_display *dpy = compositor->display();
    wl_list *clients = wl_display_get_client_list(dpy);
    wl_client *client;
    wl_client_for_each(client, clients) {
        addClient(client);
    }

    ClientsListener *listener = new ClientsListener;
    wl_display_add_client_created_listener(dpy, &listener->listener);
    listener->listener.notify = [](wl_listener *listener, void *data) {
        wl_client *client = static_cast<wl_client *>(data);
        reinterpret_cast<ClientsListener *>(listener)->inspector->addClient(client);
    };
    listener->inspector = this;

}

void WlCompositorInspector::addClient(wl_client *c)
{
    QWaylandClient *client = QWaylandClient::fromWlClient(m_compositor, c);

    QString pid = QString::number(client->processId());
    qWarning()<<"client"<<client<<pid;
    connect(client, &QObject::destroyed, [this, pid, client](QObject *) {
        m_clientsModel->removeClient(client);
    });

    m_clientsModel->addClient(client);
}

void WlCompositorInspector::setSelectedClient(int index)
{
    auto client = index >= 0 ?  m_clientsModel->client(index) : nullptr;
    if (client != m_resourcesModel->client()) {
        m_resourcesModel->setClient(client);
    }
}

}
