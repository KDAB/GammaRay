/*
  safetyfilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MODELINSPECTOR_MODELCONTENTPROXYMODEL_H
#define GAMMARAY_MODELINSPECTOR_MODELCONTENTPROXYMODEL_H

#include <common/modelroles.h>

#include <QIdentityProxyModel>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {

/*! Proxies source model content to the client.
 *  This does some safety checks for known broken sources.
 */
class ModelContentProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    enum Roles {
        DisabledRole = GammaRay::UserRole + 1,
        SelectedRole,
        IsDisplayStringEmptyRole
    };

    explicit ModelContentProxyModel(QObject *parent = nullptr);
    ~ModelContentProxyModel() override;

    /*! Will provide the selection via a custom role for rendering in ModelContentDelegate. */
    void setSelectionModel(QItemSelectionModel *selectionModel);

    void setSourceModel(QAbstractItemModel *model) override;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    void emitDataChangedForSelection(const QItemSelection &selection);

    QPointer<QItemSelectionModel> m_selectionModel;
};

}

#endif // SAFETYFILTERPROXYMODEL_H
