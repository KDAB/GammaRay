/*
  metatypesmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METATYPEBROWSER_METATYPESMODEL_H
#define GAMMARAY_METATYPEBROWSER_METATYPESMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {
class MetaTypesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MetaTypesModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void scanMetaTypes();

private:
    QVector<int> m_metaTypes;
};
}

#endif
