/*
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 * Author: Filipe Azevedo <filipe.azevedo@kdab.com>
 *
 * Licensees holding valid commercial KDAB GammaRay licenses may use this file in
 * accordance with GammaRay Commercial License Agreement provided with the Software.
 *
 * Contact info@kdab.com if any conditions of this licensing are not clear to you.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "itemdelegate.h"

#include <QApplication>

using namespace GammaRay;

ItemDelegateInterface::ItemDelegateInterface()
    : m_placeholderText(ItemDelegate::tr("(Item %r)"))
{
}

ItemDelegateInterface::ItemDelegateInterface(const QString &placeholderText)
    : m_placeholderText(placeholderText)
{
}

QString ItemDelegateInterface::placeholderText() const
{
    return m_placeholderText;
}

void ItemDelegateInterface::setPlaceholderText(const QString &placeholderText)
{
    m_placeholderText = placeholderText;
}

QSet<int> ItemDelegateInterface::placeholderColumns() const
{
    return m_placeholderColumns;
}

void ItemDelegateInterface::setPlaceholderColumns(const QSet<int> &placeholderColumns)
{
    m_placeholderColumns = placeholderColumns;
}

QString ItemDelegateInterface::defaultDisplayText(const QModelIndex &index) const
{
    QString display = index.data().toString();
    if (display.isEmpty()
        && (m_placeholderColumns.isEmpty() || m_placeholderColumns.contains(index.column()))) {
        display = QString(m_placeholderText)
                  .replace(QStringLiteral("%r"), QString::number(index.row()))
                  .replace(QStringLiteral("%c"), QString::number(index.column()));
    }
    return display;
}

const QWidget *ItemDelegateInterface::widget(const QStyleOptionViewItem &option) const
{
    const QStyleOptionViewItem &opt(option);
    return opt.widget;
}

QStyle *ItemDelegateInterface::style(const QStyleOptionViewItem &option) const
{
    const QWidget *widget = this->widget(option);
    return widget ? widget->style() : QApplication::style();
}

ItemDelegate::ItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , ItemDelegateInterface()
{
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    opt.text = defaultDisplayText(index);
    initStyleOption(&opt, index);

    const QWidget *widget = this->widget(option);
    QStyle *style = this->style(option);
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}
