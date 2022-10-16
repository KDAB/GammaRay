/*
  messagemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEMODEL_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEMODEL_H

#include <core/execution.h>
#include <common/tools/messagehandler/messagemodelroles.h>

#include <QAbstractTableModel>
#include <QStringList>
#include <QTime>
#include <QVector>

namespace GammaRay {
struct DebugMessage
{
    QtMsgType type;
    QString message;
    QTime time;
    Execution::Trace backtrace;
    QString category;
    QString file;
    QString function;
    int line;
};
}

Q_DECLARE_METATYPE(GammaRay::DebugMessage)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::DebugMessage, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

namespace GammaRay {
class MessageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MessageModel(QObject *parent = nullptr);
    ~MessageModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

public slots:
    void addMessage(const GammaRay::DebugMessage &message);

private:
    QVector<DebugMessage> m_messages;
};
}

#endif // MESSAGEMODEL_H
