/*
  modelcontentproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    enum Roles
    {
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
