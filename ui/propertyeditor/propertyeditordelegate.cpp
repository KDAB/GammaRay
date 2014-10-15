/*
  propertyeditordelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertyeditordelegate.h"
#include "propertyeditorfactory.h"

#include <QApplication>
#include <QDebug>
#include <QMatrix4x4>
#include <QPainter>

using namespace GammaRay;

PropertyEditorDelegate::PropertyEditorDelegate(QObject* parent): QStyledItemDelegate(parent)
{
    setItemEditorFactory(PropertyEditorFactory::instance());
}

PropertyEditorDelegate::~PropertyEditorDelegate()
{
}

void PropertyEditorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    editor->setProperty("displayString", index.data(Qt::DisplayRole));
    QStyledItemDelegate::setEditorData(editor, index);
}

void PropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QVariant value = index.data(Qt::EditRole);
    if (value.canConvert<QMatrix4x4>()) {
        paint(painter, option, index, value.value<QMatrix4x4>());
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QVariant value = index.data(Qt::EditRole);
    if (value.canConvert<QMatrix4x4>()) {
        return sizeHint(option, index, value.value<QMatrix4x4>());
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

void PropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index, const QMatrix4x4& matrix) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QStyleOptionViewItem opt = option;
#else
    QStyleOptionViewItemV4 opt = option;
#endif
    initStyleOption(&opt, index);
    opt.text.clear();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QRect textRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, opt.widget) + 1;
    textRect = textRect.adjusted(textMargin, 0, -textMargin, 0);

    static const int parenthesisLineWidth = 1;
    const int matrixSpacing = opt.fontMetrics.width("x");
    const int matrixHMargin = matrixSpacing / 2;
    const int parenthesisWidth = qMax(matrixHMargin, 3);

    painter->save();
    painter->setClipRect(textRect);
    painter->translate(textRect.topLeft());
    painter->setPen(opt.palette.color(opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text));
    int xOffset = 0;
    painter->drawLine(xOffset, 0, xOffset, textRect.height());
    painter->drawLine(xOffset, 0, xOffset + parenthesisWidth, 0);
    painter->drawLine(xOffset, textRect.height() - 1, xOffset + parenthesisWidth, textRect.height() - 1);
    xOffset += matrixHMargin + parenthesisLineWidth;
    for (int col = 0; col < 4; ++col) {
        const int colWidth = columnWidth(opt, matrix, col);
        for (int row = 0; row < 4; ++row) {
            const QRect r(xOffset, row * opt.fontMetrics.lineSpacing(), colWidth, opt.fontMetrics.lineSpacing());
            painter->drawText(r, Qt::AlignHCenter | Qt::AlignRight, QString::number(matrix(row, col)));
        }
        xOffset += colWidth + matrixSpacing;
    }
    xOffset += -matrixSpacing + matrixHMargin;
    painter->drawLine(xOffset, 0, xOffset, textRect.height());
    painter->drawLine(xOffset, 0, xOffset - parenthesisWidth, 0);
    painter->drawLine(xOffset, textRect.height() - 1, xOffset - parenthesisWidth, textRect.height() - 1);
    painter->restore();
}

QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index, const QMatrix4x4& matrix) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QStyleOptionViewItem opt = option;
#else
    QStyleOptionViewItemV4 opt = option;
#endif
    initStyleOption(&opt, index);

    static const int parenthesisLineWidth = 1;
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, opt.widget) + 1;

    int width = 0;
    for (int col = 0; col < 4; ++col)
        width += columnWidth(opt, matrix, col);
    width += opt.fontMetrics.width("x") * 4 + 2 * parenthesisLineWidth + 2 * textMargin;

    const int height = opt.fontMetrics.lineSpacing() * 4;

    return QSize(width, height);
}

int PropertyEditorDelegate::columnWidth(const QStyleOptionViewItem& option, const QMatrix4x4 &matrix, int column) const
{
    int width = 0;
    for (int row = 0; row < 4; ++row)
        width = qMax(width, option.fontMetrics.width(QString::number(matrix(row, column))));
    return width;
}
