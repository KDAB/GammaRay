/*
  fontmodel.cpp

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

#include "fontmodel.h"

using namespace GammaRay;

FontModel::FontModel(QObject *parent)
  : QAbstractItemModel(parent)
{
}

QList<QFont> FontModel::currentFonts() const
{
  return m_fonts;
}

void FontModel::updateFonts(const QList<QFont> &fonts)
{
  beginResetModel();
  m_fonts = fonts;
  endResetModel();
}

void FontModel::updateText(const QString &text)
{
  beginResetModel();
  m_text = text;
  endResetModel();
}

QVariant FontModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == 0) {
      return "Font";
    }
    if (section == 1) {
      return "Text Preview";
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int FontModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_fonts.size();
}

QModelIndex FontModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return QModelIndex();
  }
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }
  return createIndex(row, column);
}

QModelIndex FontModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);
  return QModelIndex();
}

int FontModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2;
}

QVariant FontModel::data(const QModelIndex &index, int role) const
{
  if (index.column() == 0) {
    if (role == Qt::DisplayRole) {
      return m_fonts.at(index.row()).family();
    }
  } else if (index.column() == 1) {
    if (role == Qt::DisplayRole) {
      return m_text.isEmpty() ? "<no text>" : m_text;
    }
    if (role == Qt::FontRole) {
      return m_fonts.at(index.row());
    }
  }

  return QVariant();
}

void FontModel::setPointSize(int size)
{
  if (m_fonts.isEmpty()) {
    return;
  }

  for (int i = 0; i < m_fonts.size(); ++i) {
    m_fonts[i].setPointSize(size);
  }

  emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
}

void FontModel::toggleItalicFont(bool italic)
{
  if (m_fonts.isEmpty()) {
    return;
  }

  for (int i = 0; i < m_fonts.size(); ++i) {
    m_fonts[i].setItalic(italic);
  }

  emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
}

void FontModel::toggleUnderlineFont(bool underline)
{
  if (m_fonts.isEmpty()) {
    return;
  }

  for (int i = 0; i < m_fonts.size(); ++i) {
    m_fonts[i].setUnderline(underline);
  }

  emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
}

void FontModel::toggleBoldFont(bool bold)
{
  if (m_fonts.isEmpty()) {
    return;
  }

  for (int i = 0; i < m_fonts.size(); ++i) {
    m_fonts[i].setBold(bold);
  }

  emit dataChanged(index(0, 1), index(rowCount() - 1, 1));
}

#include "fontmodel.moc"
