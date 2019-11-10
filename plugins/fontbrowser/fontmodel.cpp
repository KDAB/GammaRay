/*
  fontmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "fontmodel.h"

#include <QFontMetrics>
#include <QPixmap>
#include <QPainter>

using namespace GammaRay;

FontModel::FontModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_size(12)
    , m_bold(false)
    , m_italic(false)
    , m_underline(false)
{
}

QVector<QFont> FontModel::currentFonts() const
{
    return m_fonts;
}

void FontModel::updateFonts(const QVector<QFont> &fonts)
{
    if (!m_fonts.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_fonts.size() - 1);
        m_fonts.clear();
        endRemoveRows();
    }

    if (fonts.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, fonts.size() - 1);
    m_fonts = fonts;

    for (int i = 0; i < m_fonts.size(); ++i) {
        QFont &font = m_fonts[i];
        font.setPointSize(m_size);
        font.setBold(m_bold);
        font.setItalic(m_italic);
        font.setUnderline(m_underline);
    }

    endInsertRows();
}

void FontModel::updateText(const QString &text)
{
    if (text == m_text)
        return;
    m_text = text;
    fontDataChanged();
}

QVariant FontModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0)
            return "Font Family";
        else if (section == 1)
            return "Style Name";
        else if (section == 2)
            return "Text Preview";
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

int FontModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_fonts.size();
}

int FontModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant FontModel::data(const QModelIndex &index, int role) const
{
    if (index.column() == 0) {
        if (role == Qt::DisplayRole)
            return m_fonts.at(index.row()).family();
    } else if (index.column() == 1) {
        if (role == Qt::DisplayRole)
            return m_fonts.at(index.row()).styleName();
    } else if (index.column() == 2) {
        if (role == Qt::DecorationRole || role == Qt::SizeHintRole) {
            const QFont &font = m_fonts.at(index.row());
            QFontMetrics metrics(font);
            const QString text = m_text.isEmpty() ? tr("<no text>") : m_text;
            const QRect rect = metrics.boundingRect(text.left(100));
            if (role == Qt::SizeHintRole)
                return rect.size();
            QPixmap pixmap(rect.size());
            pixmap.fill(m_background);
            QPainter painter(&pixmap);
            painter.setPen(m_foreground);
            painter.setFont(font);
            painter.drawText(0, -rect.y(), text);
            return pixmap;
        }
    }

    return QVariant();
}

void FontModel::setPointSize(int size)
{
    if (size == m_size)
        return;

    m_size = size;

    for (int i = 0; i < m_fonts.size(); ++i)
        m_fonts[i].setPointSize(size);

    fontDataChanged();
}

void FontModel::toggleItalicFont(bool italic)
{
    if (italic == m_italic)
        return;

    m_italic = italic;

    for (int i = 0; i < m_fonts.size(); ++i)
        m_fonts[i].setItalic(italic);

    fontDataChanged();
}

void FontModel::toggleUnderlineFont(bool underline)
{
    if (underline == m_underline)
        return;

    m_underline = underline;

    for (int i = 0; i < m_fonts.size(); ++i)
        m_fonts[i].setUnderline(underline);

    fontDataChanged();
}

void FontModel::toggleBoldFont(bool bold)
{
    if (bold == m_bold)
        return;

    m_bold = bold;

    for (int i = 0; i < m_fonts.size(); ++i)
        m_fonts[i].setBold(bold);

    fontDataChanged();
}

void FontModel::setColors(const QColor &foreground, const QColor &background)
{
    if (foreground == m_foreground && background == m_background)
        return;

    m_foreground = foreground;
    m_background = background;

    fontDataChanged();
}

void FontModel::fontDataChanged()
{
    if (m_fonts.isEmpty())
        return;

    emit dataChanged(index(0, 2), index(rowCount() - 1, 2));
}
