/*
  sggeometrymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_SGGEOMETRYMODEL_H
#define GAMMARAY_QUICKINSPECTOR_SGGEOMETRYMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QSGGeometryNode;
class QSGGeometry;
QT_END_NAMESPACE

namespace GammaRay {
class SGVertexModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Role
    {
        IsCoordinateRole = 257,
        RenderRole = 258
    };

    explicit SGVertexModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;

    void setNode(QSGGeometryNode *node);

private:
    QSGGeometry *m_geometry;
    QSGGeometryNode *m_node;
};

class SGAdjacencyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        DrawingModeRole = 257,
        RenderRole = 258
    };
    explicit SGAdjacencyModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    void setNode(QSGGeometryNode *node);

private:
    QSGGeometry *m_geometry;
    QSGGeometryNode *m_node;
};
}

#endif // GAMMARAY_SGGEOMETRYMODEL_H
