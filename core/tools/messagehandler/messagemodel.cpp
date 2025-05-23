/*
  messagemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "messagemodel.h"

#include <common/tools/messagehandler/messagemodelroles.h>

#include <QDebug>

using namespace GammaRay;

MessageModel::MessageModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    qRegisterMetaType<DebugMessage>();
}

MessageModel::~MessageModel() = default;

void MessageModel::addMessage(const DebugMessage &message)
{
    /// WARNING: do not trigger *any* kind of debug output here
    ///          this would trigger an infinite loop and hence crash!

    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages << message;
    endInsertRows();
}

int MessageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return MessageModelColumn::COUNT;
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.size();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    const DebugMessage &msg = m_messages.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case MessageModelColumn::Message:
            return msg.message;
        case MessageModelColumn::Time:
            return msg.time.toString();
        case MessageModelColumn::Category:
            return msg.category;
        case MessageModelColumn::Function:
            return msg.function;
        case MessageModelColumn::File:
            return msg.file;
        }
    } else if (role == MessageModelRole::Sort) {
        switch (index.column()) {
        case MessageModelColumn::Time:
            return msg.time;
        case MessageModelColumn::Message:
            return msg.message;
        case MessageModelColumn::Category:
            return msg.category;
        case MessageModelColumn::Function:
            return msg.function;
        case MessageModelColumn::File:
            return QString::fromLatin1("%1:%2").arg(msg.file).arg(msg.line);
        }
    } else if (role == MessageModelRole::Type && index.column() == 0) {
        return static_cast<int>(msg.type);
    } else if (role == MessageModelRole::Line && index.column() == MessageModelColumn::File) {
        return msg.line;
    } else if (role == MessageModelRole::Backtrace) {
        return QVariant::fromValue(msg.backtrace);
    }

    return QVariant();
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case MessageModelColumn::Message:
            return tr("Message");
        case MessageModelColumn::Time:
            return tr("Time");
        case MessageModelColumn::Category:
            return tr("Category");
        case MessageModelColumn::Function:
            return tr("Function");
        case MessageModelColumn::File:
            return tr("Source");
        }
    }

    return QVariant();
}
