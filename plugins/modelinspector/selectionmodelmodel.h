/*
  selectionmodelmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_SELECTIONMODELMODEL_H
#define GAMMARAY_SELECTIONMODELMODEL_H

#include <core/objectmodelbase.h>

#include <QVector>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {

/*! Selection models for a specific model. */
class SelectionModelModel : public ObjectModelBase<QAbstractTableModel>
{
    Q_OBJECT
public:
    explicit SelectionModelModel(QObject *parent = nullptr);
    ~SelectionModelModel() override;

    /*! show only selection models for @p model */
    void setModel(QAbstractItemModel *model);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void objectCreated(QObject *obj);
    void objectDestroyed(QObject *obj);

private slots:
    void sourceModelChanged();
    void selectionChanged();

private:
    QVector<QItemSelectionModel *> m_selectionModels;
    QVector<QItemSelectionModel *> m_currentSelectionModels;
    QAbstractItemModel *m_model;
};
}

#endif // GAMMARAY_SELECTIONMODELMODEL_H
