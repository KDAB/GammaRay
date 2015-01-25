/*
  abstractconnectionsmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTINSPECTOR_ABSTRACTCONNECTIONSMODEL_H
#define GAMMARAY_OBJECTINSPECTOR_ABSTRACTCONNECTIONSMODEL_H

#include <QAbstractTableModel>

#include <QPointer>
#include <QVector>

namespace GammaRay {

/** Common base class for the inbound and outbound connections models. */
class AbstractConnectionsModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit AbstractConnectionsModel(QObject *parent = 0);
    ~AbstractConnectionsModel();

    virtual void setObject(QObject *object) = 0;

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QMap< int, QVariant > itemData(const QModelIndex &index) const;

  protected:
    static QString displayString(QObject *object, int methodIndex);
    static QString displayString(QObject *object);

    static int signalIndexToMethodIndex(QObject *object, int signalIndex);

  protected:
    QPointer<QObject> m_object;
    struct Connection {
      QPointer<QObject> endpoint;
      int signalIndex;
      int slotIndex;
      int type;
    };
    QVector<Connection> m_connections;

  private:
    bool isDuplicate(const Connection &conn) const;
    bool isDirectCrossThreadConnection(const Connection &conn) const;
};

}

#endif // GAMMARAY_ABSTRACTCONNECTIONSMODEL_H
