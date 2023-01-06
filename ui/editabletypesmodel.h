/*
  editabletypesmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_EDITABLETYPESMODEL_H
#define GAMMARAY_EDITABLETYPESMODEL_H

#include <QAbstractListModel>
#include <QVector>

namespace GammaRay {
/** All types we have edit widgets for. */
class EditableTypesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit EditableTypesModel(QObject *parent = nullptr);
    ~EditableTypesModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<int> m_types;
};
}

#endif // GAMMARAY_EDITABLETYPESMODEL_H
