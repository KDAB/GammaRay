/*
  networkselectionmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
