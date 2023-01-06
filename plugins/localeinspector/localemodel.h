/*
  localemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEMODEL_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QLocale>

namespace GammaRay {
class LocaleDataAccessorRegistry;
struct LocaleDataAccessor;

class LocaleModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LocaleModel(LocaleDataAccessorRegistry *registry, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    void init();

private slots:
    void accessorAdded();
    void accessorRemoved(int idx);

private:
    QVector<QLocale> m_locales;
    QVector<LocaleDataAccessor *> m_localeData;
    LocaleDataAccessorRegistry *m_registry;
};
}

#endif
