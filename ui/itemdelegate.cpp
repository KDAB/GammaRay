/*
  itemdelegate.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

const QWidget *ItemDelegateInterface::widget(const QStyleOptionViewItem &option)
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
