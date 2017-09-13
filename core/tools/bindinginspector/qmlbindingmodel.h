/*
  qmlbindingmodel.h

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

#ifndef GAMMARAY_QMLBINDINGMODEL_H
#define GAMMARAY_QMLBINDINGMODEL_H

#include "gammaray_core_export.h"

#include <QAbstractItemModel>
#include <QVector>
#include <memory>
#include <vector>
#include <QPointer>

class QQmlBinding;

namespace GammaRay {

struct BindingNode;
class AbstractBindingProvider;

class GAMMARAY_CORE_EXPORT QmlBindingModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    explicit QmlBindingModel(QObject *parent = Q_NULLPTR);
    ~QmlBindingModel();

    bool setObject(QObject *obj);

    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex & parent) const override;
    QModelIndex parent(const QModelIndex & child) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    static GAMMARAY_CORE_EXPORT void registerBindingProvider(std::unique_ptr<AbstractBindingProvider> provider);

private:
    QModelIndex findEquivalent(const std::vector<std::unique_ptr<BindingNode>> &container, BindingNode *bindingNode) const;
    void refresh(BindingNode *oldBindingNode, const QModelIndex &index);
    void bindingChanged(BindingNode *node);
    void findDependenciesFor(BindingNode *node);
    static bool lessThan(const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b);

    QPointer<QObject> m_obj;
    std::vector<std::unique_ptr<BindingNode>> m_bindings;
    static std::vector<std::unique_ptr<AbstractBindingProvider>> s_providers;
};

}

#endif // GAMMARAY_QMLBINDINGMODEL_H
