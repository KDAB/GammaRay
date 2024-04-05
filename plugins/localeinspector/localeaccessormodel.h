/*
  localeaccessormodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEACCESSORMODEL_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEACCESSORMODEL_H

#include "common/modelroles.h"

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {
class LocaleDataAccessorRegistry;
struct LocaleDataAccessor;

class LocaleAccessorModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum CustomRoles
    {
        AccessorRole = UserRole + 1
    };
    Q_ENUM(CustomRoles)

    explicit LocaleAccessorModel(LocaleDataAccessorRegistry *registry, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

private:
    QVector<LocaleDataAccessor *> enabledAccessors;
    LocaleDataAccessorRegistry *m_registry;
};
}

#endif
