/*
  quickitemdelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
