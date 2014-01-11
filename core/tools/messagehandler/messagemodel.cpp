/*
  messagemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include <QDebug>

using namespace GammaRay;

QString typeToString(QtMsgType type)
{
  switch(type) {
    case QtDebugMsg:
      return QObject::tr("Debug");
    case QtWarningMsg:
      return QObject::tr("Warning");
    case QtCriticalMsg:
      return QObject::tr("Critical");
    case QtFatalMsg:
      return QObject::tr("Fatal");
    default:
      return QObject::tr("Unknown"); // never reached in theory
  }
}

MessageModel::MessageModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  qRegisterMetaType<DebugMessage>();
}

MessageModel::~MessageModel()
{

}

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
  return COLUMN_COUNT;
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  return m_messages.count();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount()) {
    return QVariant();
  }

  const DebugMessage &msg = m_messages.at(index.row());

  if (role == Qt::DisplayRole) {
    if (index.column() == TypeColumn) {
      return typeToString(msg.type);
    } else if (index.column() == MessageColumn) {
      ///TODO: elide
      return msg.message;
    } else if (index.column() == TimeColumn) {
      return msg.time.toString();
    }
  } else if (role == Qt::ToolTipRole) {
    if (!msg.backtrace.isEmpty()) {
      QString bt;
      int i = 0;
      foreach (const QString &frame, msg.backtrace) {
        bt += QString("#%1: %2\n").arg(i, 2).arg(frame);
        ++i;
      }
      return tr("<qt><dl>"
                  "<dt><b>Type:</b></dt><dd>%1</dd>"
                  "<dt><b>Time:</b></dt><dd>%2</dd>"
                  "<dt><b>Message:</b></dt><dd>%3</dd>"
                  "<dt><b>Backtrace:</b></dt><dd><pre>%4</pre></dd>"
                "</dl></qt>").arg(typeToString(msg.type), msg.time.toString(), msg.message, bt);
    } else {
      return tr("<qt><dl>"
                  "<dt><b>Type:</b></dt><dd>%1</dd>"
                  "<dt><b>Time:</b></dt><dd>%2</dd>"
                  "<dt><b>Message:</b></dt><dd>%3</dd>"
                "</dl></qt>").arg(typeToString(msg.type), msg.time.toString(), msg.message);
    }
  }

  return QVariant();
}

QVariant MessageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    if (section == TypeColumn) {
      return tr("Type");
    } else if (section == MessageColumn) {
      return tr("Message");
    } else if (section == TimeColumn) {
      return tr("Time");
    }
  }

  return QVariant();
}

