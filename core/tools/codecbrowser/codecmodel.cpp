/*
  codecmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "codecmodel.h"

#include <QDebug>

using namespace GammaRay;

AllCodecsModel::AllCodecsModel(QObject *parent)
  : QAbstractItemModel(parent)
{
}

int AllCodecsModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2;
}

QVariant AllCodecsModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return QTextCodec::availableCodecs().at(index.row());
    }
    if (index.column() == 1) {
      QList<QByteArray> aliases =
        QTextCodec::codecForName(QTextCodec::availableCodecs().at(index.row()))->aliases();

      QString result;
      int size = aliases.size();
      int i = 0;
      foreach (const QByteArray &ba, aliases) {
        result.append(ba);

        ++i;
        if (i != size) {
          result.append(", ");
        }
      }
      return result;
    }
  }
  return QVariant();
}

QVariant AllCodecsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0) {
      return "Codec";
    } else if (section == 1) {
      return "Aliases";
    }
  }
  return QVariant();
}

QModelIndex AllCodecsModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return QModelIndex();
  }
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }
  return createIndex(row, column);
}

QModelIndex AllCodecsModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);
  return QModelIndex();
}

int AllCodecsModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return QTextCodec::availableCodecs().size();
}

SelectedCodecsModel::SelectedCodecsModel(QObject *parent)
  : QAbstractItemModel(parent)
{
}

void SelectedCodecsModel::setCodecs(const QStringList &codecs)
{
  beginResetModel();
  m_codecs = codecs;
  endResetModel();
}

QStringList SelectedCodecsModel::currentCodecs() const
{
  return m_codecs;
}

void SelectedCodecsModel::updateText(const QString &text)
{
  beginResetModel();
  m_text = text;
  endResetModel();
}

QVariant SelectedCodecsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0) {
      return "Codec";
    }
    if (section == 1) {
      return "Data";
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int SelectedCodecsModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_codecs.size();
}

QModelIndex SelectedCodecsModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return QModelIndex();
  }
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }
  return createIndex(row, column);
}

QModelIndex SelectedCodecsModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);
  return QModelIndex();
}

int SelectedCodecsModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2;
}

QVariant SelectedCodecsModel::data(const QModelIndex &index, int role) const
{
  if (index.column() == 0) {
    if (role == Qt::DisplayRole) {
      return m_codecs.at(index.row());
    }
  } else if (index.column() == 1) {
    if (role == Qt::DisplayRole) {
      const QByteArray ba =
        QTextCodec::codecForName(m_codecs.at(index.row()).toLatin1())->fromUnicode(m_text);
//       QString result;
//       foreach ()
      return ba.toHex();
    }
  }

  return QVariant();
}

#include "codecmodel.moc"
