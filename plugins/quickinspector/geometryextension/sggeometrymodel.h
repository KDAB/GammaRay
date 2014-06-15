/*
  sggeometrymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

class QSGGeometryNode;
class QSGGeometry;

namespace GammaRay {

class SGGeometryModel : public QAbstractTableModel
{
  public:

    enum Role {
      IsCoordinateRole = 257,
      RenderRole = 258
    };

    explicit SGGeometryModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    void setNode(QSGGeometryNode *node);

    template <typename T>
    static QStringList toStringList(void *data, int size) {
      QStringList list;
      T* typedData = static_cast<T*>(data);
      for (int i = 0; i < size; i++) {
        list << QString::number(*typedData);
        ++typedData;
      }
      return list;
    }
    template <typename T>
    static QVariantList toVariantList(void *data, int size) {
      QVariantList list;
      T* typedData = static_cast<T*>(data);
      for (int i = 0; i < size; i++) {
        list << QVariant::fromValue<T>(*typedData);
        ++typedData;
      }
      return list;
    }

  private:
    QSGGeometry *m_geometry;
    QSGGeometryNode *m_node;
};

}

#endif // GAMMARAY_SGGEOMETRYMODEL_H
