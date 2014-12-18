#include "serverproxymodel.h"

#include <QDebug>

using namespace GammaRay;

ServerProxyModel::ServerProxyModel(QObject* parent): QSortFilterProxyModel(parent)
{
}

void ServerProxyModel::addRole(int role)
{
    m_extraRoles.push_back(role);
}

QMap< int, QVariant > ServerProxyModel::itemData(const QModelIndex& index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    auto d = sourceModel()->itemData(sourceIndex);
    foreach (int role, m_extraRoles)
        d.insert(role, sourceIndex.data(role));
    return d;
}
