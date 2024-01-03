/*
  abstractconnectionsmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    explicit AbstractConnectionsModel(QObject *parent = nullptr);
    ~AbstractConnectionsModel() override;

    virtual void setObject(QObject *object) = 0;

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

    struct Connection
    {
        QPointer<QObject> endpoint;
        int signalIndex;
        int slotIndex;
        int type;
    };

protected:
    static QString displayString(QObject *object, int methodIndex);
    static QString displayString(QObject *object);

    static int signalIndexToMethodIndex(QObject *object, int signalIndex);

    void clear();
    void setConnections(const QVector<Connection> &connections);

public:
    static bool isDuplicate(const QVector<Connection> &connections, const Connection &conn);
    static bool isDirectCrossThreadConnection(QObject *object, const Connection &conn);

protected:
    QPointer<QObject> m_object;
    QVector<Connection> m_connections;

private:
    bool isDuplicate(const Connection &conn) const;
    bool isDirectCrossThreadConnection(const Connection &conn) const;
};
}

#endif // GAMMARAY_ABSTRACTCONNECTIONSMODEL_H
