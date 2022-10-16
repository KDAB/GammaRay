/*
  itemdelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ITEMDELEGATE_H
#define GAMMARAY_ITEMDELEGATE_H

#include "gammaray_ui_export.h"

#include <QStyledItemDelegate>
#include <QSet>

namespace GammaRay {
/** @brief A simple interface that avoid empty display role texts.
 */
class GAMMARAY_UI_EXPORT ItemDelegateInterface
{
public:
    ItemDelegateInterface();
    explicit ItemDelegateInterface(const QString &placeholderText);

    // You can put 2 placeholders for row/column using %r and %c
    QString placeholderText() const;
    void setPlaceholderText(const QString &placeholderText);

    // which columns do show place holders, default all (empty)
    QSet<int> placeholderColumns() const;
    void setPlaceholderColumns(const QSet<int> &placeholderColumns);

protected:
    QString defaultDisplayText(const QModelIndex &index) const;

    static const QWidget *widget(const QStyleOptionViewItem &option);
    QStyle *style(const QStyleOptionViewItem &option) const;

private:
    QString m_placeholderText;
    QSet<int> m_placeholderColumns;
};

/** @brief A simple delegate that avoid empty display role texts.
 */
class GAMMARAY_UI_EXPORT ItemDelegate : public QStyledItemDelegate, public ItemDelegateInterface
{
    Q_OBJECT

public:
    explicit ItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
};
} // Namespace GammaRay

#endif // GAMMARAY_ITEMDELEGATE_H
