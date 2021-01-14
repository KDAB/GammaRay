/*
  sggeometrymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    enum Role {
        IsCoordinateRole = 257,
        RenderRole = 258
    };

    explicit SGVertexModel (QObject *parent = nullptr);
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
    enum Role {
        DrawingModeRole = 257,
        RenderRole = 258
    };
    explicit SGAdjacencyModel (QObject *parent = nullptr);
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
