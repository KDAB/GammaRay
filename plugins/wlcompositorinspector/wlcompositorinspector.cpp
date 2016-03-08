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

enum {
    ResourcesRole = Qt::UserRole + 1,
};

namespace GammaRay
{

class ResourcesModel : public QAbstractItemModel
{
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

    ResourcesModel(QWaylandClient *client)
    {
        struct Listener {
            wl_listener l;
            ResourcesModel *m;
        };

        auto *listener = new Listener;
        wl_client_add_resource_created_listener(client->client(), &listener->l);
        listener->m = this;
        listener->l.notify = [](wl_listener *listener, void *data) {
            wl_resource *resource = static_cast<wl_resource *>(data);
            ResourcesModel *model = reinterpret_cast<Listener *>(listener)->m;
            model->addResource(resource);
        };

        wl_client_for_each_resource(client->client(), [](wl_resource *res, void *ud) {
            ResourcesModel *model = static_cast<ResourcesModel *>(ud);
            model->addResource(res);
            return WL_ITERATOR_CONTINUE;
        }, this);
    }

    ~ResourcesModel()
    {
        foreach (Resource *res, m_resources) {
            destroy(res);
        }
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
        Resource *parentres = static_cast<Resource *>(parent.internalPointer());
        Resource *res = parentres ? parentres->children.at(row) : m_resources.at(row);
        return createIndex(row, column, res);
    }

    QModelIndex parent(const QModelIndex &idx) const override
    {
        Resource *res = static_cast<Resource *>(idx.internalPointer());
        if (res->parent) {
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
        delete r;
        endRemoveRows();
    }

    int rowCount(const QModelIndex &index) const override
    {
        Resource *res = static_cast<Resource *>(index.internalPointer());
        return res ? res->children.count() : m_resources.count();
    }

    int columnCount(const QModelIndex &) const override
    {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
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
};

class ClientsModel : public QAbstractTableModel
{
public:
    enum Columns {
        PidColumn,
        CommandColumn,
        EndColumn
    };

    struct Client {
        QWaylandClient *client;
        QString modelName;
        ResourcesModel *model;

        inline bool operator==(QWaylandClient *c) const { return client == c; }
    };

    explicit ClientsModel(ProbeInterface *probe, QObject *parent)
        : QAbstractTableModel(parent)
        , m_probe(probe)
    {
    }

    void addClient(QWaylandClient *client)
    {
        QString modelName = QString("com.kdab.GammaRay.WaylandCompositorClient%1ResourcesModel").arg(client->processId());
        auto *model = new ResourcesModel(client);
        m_probe->registerModel(modelName, model);

        beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count());
        m_clients.append({ client, modelName, model });
        endInsertRows();
    }

    void removeClient(QWaylandClient *client)
    {
        const Client *modelClient;
        int index = -1;
        for (int i = 0; i < m_clients.count(); ++i) {
            if (m_clients.at(i) == client) {
                index = i;
                modelClient = &m_clients.at(i);
                break;
            }
        }

        if (index == -1) {
            return;
        }

        delete modelClient->model;
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
        if (role != Qt::DisplayRole && role != ResourcesRole) {
            return QVariant();
        }

        const Client &client = m_clients.at(index.row());

        if (role == ResourcesRole) {
            return client.modelName;
        }

        switch (index.column()) {
            case PidColumn:
                return client.client->processId();
            case CommandColumn: {
                auto pid = client.client->processId();
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
        insertRole(ResourcesRole);
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

    QVector<Client> m_clients;
    ProbeInterface *m_probe;
};

WlCompositorInspector::WlCompositorInspector(ProbeInterface* probe, QObject* parent)
                     : QObject(parent)
{
    qWarning()<<"init probe"<<probe->objectTreeModel()<<probe->probe();

    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QWaylandCompositor, QWaylandObject);

    m_clientsModel = new ClientsModel(probe, this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.WaylandCompositorClientsModel"), m_clientsModel);

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
    m_clients << pid;
    connect(client, &QObject::destroyed, [this, pid, client](QObject *) {
        m_clients.removeOne(pid);
        m_clientsModel->removeClient(client);
    });

    m_clientsModel->addClient(client);
}

}
