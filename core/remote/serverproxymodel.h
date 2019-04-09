/*
  serverproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SERVERPROXYMODEL_H
#define GAMMARAY_SERVERPROXYMODEL_H

#include <common/modelevent.h>

#include <QCoreApplication>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVector>

namespace GammaRay {
/** Sort/filter proxy model for server-side use to pass through extra roles in itemData().
 *  Every remoted proxy model should be wrapped into this template, unless you already have
 *  a special implementation for itemData() handling this.
 */
template<typename BaseProxy> class ServerProxyModel : public BaseProxy
{
public:
    explicit ServerProxyModel(QObject *parent = nullptr)
        : BaseProxy(parent)
        , m_sourceModel(nullptr)
        , m_active(false)
    {
    }

    /** Additional roles used from the source model for transfer to the client. */
    void addRole(int role)
    {
        m_extraRoles.push_back(role);
    }

    /** Additional roles used from the proxy model itself for transfer to the client.
     *  This is useful if BaseProxy overrides data().
     */
    void addProxyRole(int role)
    {
        m_extraProxyRoles.push_back(role);
    }

    QMap<int, QVariant> itemData(const QModelIndex &index) const override
    {
        const QModelIndex sourceIndex = BaseProxy::mapToSource(index);
        auto d = BaseProxy::sourceModel()->itemData(sourceIndex);
        for (int role : m_extraRoles)
            d.insert(role, sourceIndex.data(role));
        for (int role : m_extraProxyRoles)
            d.insert(role, index.data(role));
        return d;
    }

    void setSourceModel(QAbstractItemModel *sourceModel) override
    {
        m_sourceModel = sourceModel;
        if (m_active && sourceModel) {
            Model::used(sourceModel);
            BaseProxy::setSourceModel(sourceModel);
        }
    }

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override
    {
        Model::used(this);
        return BaseProxy::index(row, column, parent);
    }

protected:
    void customEvent(QEvent *event) override
    {
        if (event->type() == ModelEvent::eventType()) {
            auto mev = static_cast<ModelEvent *>(event);
            m_active = mev->used();
            if (m_sourceModel) {
                QCoreApplication::sendEvent(m_sourceModel, event);
                if (mev->used() && BaseProxy::sourceModel() != m_sourceModel)
                    BaseProxy::setSourceModel(m_sourceModel);
                else if (!mev->used())
                    BaseProxy::setSourceModel(nullptr);
            }
        }
        BaseProxy::customEvent(event);
    }

private:
    QVector<int> m_extraRoles;
    QVector<int> m_extraProxyRoles;
    QPointer<QAbstractItemModel> m_sourceModel;
    bool m_active;
};
}

#endif // GAMMARAY_SERVERPROXYMODEL_H
