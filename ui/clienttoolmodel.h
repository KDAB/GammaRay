/*
  clienttoolmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTTOOLMODEL_H
#define GAMMARAY_CLIENTTOOLMODEL_H

#include <QAbstractListModel>
#include <QItemSelectionModel>

namespace GammaRay {

class ClientToolManager;

/*! Model of all selectable client tools. */
class ClientToolModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ClientToolModel(ClientToolManager *manager);
    ~ClientToolModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private slots:
    void startReset();
    void finishReset();
    void toolEnabled(int toolIndex);

private:
    ClientToolManager *m_toolManager;
};

/*! Selection model that automatically syncs ClientToolModel with ClientToolManager. */
class ClientToolSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    explicit ClientToolSelectionModel(ClientToolManager *manager);
    ~ClientToolSelectionModel() override;

private slots:
    void selectTool(int index);
    void selectDefaultTool();

private:
    ClientToolManager *m_toolManager;
};

}

#endif // GAMMARAY_CLIENTTOOLMODEL_H
