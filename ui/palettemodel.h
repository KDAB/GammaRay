/*
  palettemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PALETTEMODEL_H
#define GAMMARAY_PALETTEMODEL_H

#include <QAbstractItemModel>
#include <QPalette>

namespace GammaRay {
/**
 * @brief Model showing the content of a QPalette.
 */
class PaletteModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PaletteModel(QObject *parent = nullptr);

    QPalette palette() const;
    void setPalette(const QPalette &palette);
    void setEditable(bool editable);

    ///@cond override
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    ///@endcond

private:
    QPalette m_palette;
    bool m_editable;
};
}

#endif // GAMMARAY_PALETTEMODEL_H
