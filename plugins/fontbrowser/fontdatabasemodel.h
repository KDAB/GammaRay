/*
  fontdatabasemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_FONTDATABASEMODEL_H
#define GAMMARAY_FONTDATABASEMODEL_H

#include <QAbstractItemModel>
#include <QString>
#include <QVector>

namespace GammaRay {
/** Font families and font styles. */
class FontDatabaseModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FontDatabaseModel(QObject *parent = nullptr);
    ~FontDatabaseModel() override;

    enum Columns
    {
        Label,
        Weight,
        Bold,
        Italic,
        Scalable,
        SmoothlyScalable,
        BitmapScalable,
        SmoothSizes,
        NUM_COLUMNS
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QHash<int, QByteArray> roleNames() const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

private:
    void ensureModelPopulated() const;
    void populateModel();

    static QString smoothSizeString(const QString &family, const QString &style);

    QVector<QString> m_families;
    QVector<QVector<QString>> m_styles;
};
}

#endif // GAMMARAY_FONTDATABASEMODEL_H
