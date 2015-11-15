/*
  propertyeditordelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertyeditordelegate.h"
#include "propertyeditorfactory.h"

#include <QApplication>
#include <QDebug>
#include <QMatrix4x4>
#include <QPainter>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

using namespace GammaRay;

namespace {
template <typename T> struct matrix_trait {};
template <> struct matrix_trait<QMatrix4x4> {
    static const int rows = 4;
    static const int columns = 4;
    static qreal value(const QMatrix4x4 &matrix, int r, int c) { return matrix(r, c); }
};

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
template <> struct matrix_trait<QVector2D> {
    static const int rows = 2;
    static const int columns = 1;
    static qreal value(const QVector2D &vec, int r, int) { return vec[r]; }
};

template <> struct matrix_trait<QVector3D> {
  static const int rows = 3;
  static const int columns = 1;
  static qreal value(const QVector3D &vec, int r, int) { return vec[r]; }
};

template <> struct matrix_trait<QVector4D> {
  static const int rows = 4;
  static const int columns = 1;
  static qreal value(const QVector4D &vec, int r, int) { return vec[r]; }
};
#endif

}

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    } else if (value.canConvert<QVector2D>()) {
        paint(painter, option, index, value.value<QVector2D>());
    } else if (value.canConvert<QVector3D>()) {
      paint(painter, option, index, value.value<QVector3D>());
    } else if (value.canConvert<QVector4D>()) {
      paint(painter, option, index, value.value<QVector4D>());
#endif
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QVariant value = index.data(Qt::EditRole);
    if (value.canConvert<QMatrix4x4>()) {
        return sizeHint(option, index, value.value<QMatrix4x4>());
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    } else if (value.canConvert<QVector2D>()) {
        return sizeHint(option, index, value.value<QVector2D>());
    } else if (value.canConvert<QVector3D>()) {
      return sizeHint(option, index, value.value<QVector3D>());
    } else if (value.canConvert<QVector4D>()) {
      return sizeHint(option, index, value.value<QVector4D>());
#endif
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

template <typename Matrix>
void PropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index, const Matrix& matrix) const
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
    const int textHMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, opt.widget) + 1;
    static const int textVMargin = 1;
    textRect = textRect.adjusted(textHMargin, textVMargin, -textHMargin, -textVMargin);

    static const int parenthesisLineWidth = 1;
    const int matrixSpacing = opt.fontMetrics.width(QStringLiteral("x"));
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
    for (int col = 0; col < matrix_trait<Matrix>::columns; ++col) {
        const int colWidth = columnWidth(opt, matrix, col);
        for (int row = 0; row < matrix_trait<Matrix>::rows; ++row) {
            const QRect r(xOffset, row * opt.fontMetrics.lineSpacing(), colWidth, opt.fontMetrics.lineSpacing());
            painter->drawText(r, Qt::AlignHCenter | Qt::AlignRight, QString::number(matrix_trait<Matrix>::value(matrix, row, col)));
        }
        xOffset += colWidth + matrixSpacing;
    }
    xOffset += -matrixSpacing + matrixHMargin;
    painter->drawLine(xOffset, 0, xOffset, textRect.height());
    painter->drawLine(xOffset, 0, xOffset - parenthesisWidth, 0);
    painter->drawLine(xOffset, textRect.height() - 1, xOffset - parenthesisWidth, textRect.height() - 1);
    painter->restore();
}

template <typename Matrix>
QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index, const Matrix& matrix) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QStyleOptionViewItem opt = option;
#else
    QStyleOptionViewItemV4 opt = option;
#endif
    initStyleOption(&opt, index);

    static const int parenthesisLineWidth = 1;
    const int textHMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, opt.widget) + 1;
    static const int textVMargin = 1;

    int width = 0;
    for (int col = 0; col < matrix_trait<Matrix>::columns; ++col)
        width += columnWidth(opt, matrix, col);
    width += opt.fontMetrics.width(QStringLiteral("x")) * matrix_trait<Matrix>::columns + 2 * parenthesisLineWidth + 2 * textHMargin;

    const int height = opt.fontMetrics.lineSpacing() * matrix_trait<Matrix>::rows + 2* textVMargin;

    return QSize(width, height);
}

template <typename Matrix>
int PropertyEditorDelegate::columnWidth(const QStyleOptionViewItem& option, const Matrix &matrix, int column) const
{
    int width = 0;
    for (int row = 0; row < matrix_trait<Matrix>::rows; ++row)
        width = qMax(width, option.fontMetrics.width(QString::number(matrix_trait<Matrix>::value(matrix, row, column))));
    return width;
}
