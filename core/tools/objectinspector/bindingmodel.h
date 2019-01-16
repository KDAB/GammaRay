/*
  bindingmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_BINDINGMODEL_H
#define GAMMARAY_BINDINGMODEL_H

// Own

// Qt
#include <QAbstractItemModel>
#include <QPointer>

// Std
#include <memory>
#include <vector>

class BindingInspectorTest;

namespace GammaRay {

class BindingNode;
class AbstractBindingProvider;

class BindingModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum {
        NameColumn = 0,
        ValueColumn = 1,
        DepthColumn = 2,
        LocationColumn = 3
    };

    explicit BindingModel (QObject *parent = nullptr);
    ~BindingModel() override;

    void setObject(QObject *obj, std::vector<std::unique_ptr<BindingNode>> &bindings);

    void refresh(int row, std::vector<std::unique_ptr<BindingNode>> &&newDependencies);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    void aboutToClear();
    void cleared();

private:
    QModelIndex findEquivalent(const std::vector<std::unique_ptr<BindingNode>> &container, BindingNode *bindingNode) const;
    void refresh(BindingNode *oldBindingNode, std::vector<std::unique_ptr<BindingNode>> &&newDependencies, const QModelIndex &index);
    void bindingChanged(BindingNode *node);
    void findDependenciesFor(BindingNode *node);
    static bool lessThan(const std::unique_ptr<BindingNode> &a, const std::unique_ptr<BindingNode> &b);

    QPointer<QObject> m_obj;
    std::vector<std::unique_ptr<BindingNode>> *m_bindings;
};

}

#endif // GAMMARAY_BINDINGMODEL_H
