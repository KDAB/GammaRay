/*
  quickitemdelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMDELEGATE_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMDELEGATE_H

#include <QModelIndex>
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class QAbstractItemView;
QT_END_NAMESPACE

namespace GammaRay {
class QuickItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit QuickItemDelegate(QAbstractItemView *view);

public Q_SLOTS:
    void setTextColor(const QVariant &textColor, const QPersistentModelIndex &index);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    QHash<QModelIndex, QColor> m_colors;
    QAbstractItemView *m_view;
};
}

Q_DECLARE_METATYPE(QPersistentModelIndex)

#endif // QUICKITEMDELEGATE_H
