/*
  bindingmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    enum
    {
        NameColumn = 0,
        ValueColumn = 1,
        DepthColumn = 2,
        LocationColumn = 3
    };

    explicit BindingModel(QObject *parent = nullptr);
    ~BindingModel() override;

    void setObject(QObject *obj, std::vector<std::unique_ptr<BindingNode>> &bindings);

    void refresh(int row, std::vector<std::unique_ptr<BindingNode>> &&newDependencies);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
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
