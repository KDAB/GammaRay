/*
  messagemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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
    ///WARNING: do not trigger *any* kind of debug output here
    ///         this would trigger an infinite loop and hence crash!

    beginInsertRows(QModelIndex(), m_messages.count(), m_messages.count());
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

    return m_messages.count();
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
