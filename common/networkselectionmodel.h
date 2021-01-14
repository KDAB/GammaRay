/*
  networkselectionmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_NETWORKSELECTIONMODEL_H
#define GAMMARAY_NETWORKSELECTIONMODEL_H

#include <QItemSelectionModel>
#include "protocol.h"

namespace GammaRay {
class Message;

/** Base class for network-transparent item selection models, do not use directly. */
class NetworkSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    ~NetworkSelectionModel() override;

    void select(const QItemSelection &selection,
                QItemSelectionModel::SelectionFlags command) override;
    using QItemSelectionModel::select;

protected:
    explicit NetworkSelectionModel(const QString &objectName, QAbstractItemModel *model,
                                   QObject *parent = nullptr);
    virtual bool isConnected() const;

    QString m_objectName;
    Protocol::ObjectAddress m_myAddress;

protected slots:
    void requestSelection();
    void sendSelection();
    void applyPendingSelection();

private:
    static Protocol::ItemSelection readSelection(const Message &msg);
    bool translateSelection(const Protocol::ItemSelection &selection,
                            QItemSelection &qselection) const;

private slots:
    void newMessage(const GammaRay::Message &msg);

    void slotCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

protected slots:
    void clearPendingSelection();

private:
    Protocol::ItemSelection m_pendingSelection;
    SelectionFlags m_pendingCommand;
    bool m_handlingRemoteMessage;
};
}

Q_DECLARE_METATYPE(QItemSelectionModel::SelectionFlag)
Q_DECLARE_METATYPE(QItemSelectionModel::SelectionFlags)

#endif // GAMMARAY_NETWORKSELECTIONMODEL_H
