/*
  buffermodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_BUFFERMODEL_H
#define GAMMARAY_BUFFERMODEL_H

#include "qt3dgeometryextensioninterface.h"

#include <QAbstractTableModel>

namespace GammaRay {
class BufferModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BufferModel(QObject *parent = nullptr);
    ~BufferModel();

    void setGeometryData(const Qt3DGeometryData &data);
    void setBufferIndex(int index);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    void updateAttributes();
    void updateAttribute(const Qt3DGeometryAttributeData &attrData);

    Qt3DGeometryData m_data;
    struct ColumnData
    {
        QString name;
        Qt3DRender::QAttribute::VertexBaseType type;
        uint offset;
        uint stride;
    };
    QVector<ColumnData> m_attrs;

    QByteArray m_buffer;
    int m_bufferIndex;
    int m_rowSize;
};
}

#endif // GAMMARAY_BUFFERMODEL_H
