/*
  propertyeditordelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTYEDITORDELEGATE_H
#define GAMMARAY_PROPERTYEDITORDELEGATE_H

#include "gammaray_ui_export.h"
#include <QStyledItemDelegate>

QT_BEGIN_NAMESPACE
class QMatrix4x4;
QT_END_NAMESPACE

namespace GammaRay {

/*! Delegate for rendering and editing QVariant property values.
 *  Compared to the built-in functionality this provides support for additional types,
 *  such as:
 *  - rendering of vector and matrix types in their mathematical form
 *  - editing of flags and enums resolved via EnumRepository
 *  - editing support mathematical types, palettes, fonts, colors, etc.
 *
 *  It's recommended to use this on any cell containing target-provided QVariants,
 *  no matter whether it's read-only or read-write.
 */
class GAMMARAY_UI_EXPORT PropertyEditorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PropertyEditorDelegate(QObject *parent);
    ~PropertyEditorDelegate() override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    template<typename Matrix>
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,
               const Matrix &matrix) const;
    template<typename Matrix>
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index,
                   const Matrix &matrix) const;
    template<typename Matrix>
    int columnWidth(const QStyleOptionViewItem &option, const Matrix &matrix, int column) const;
};
}

#endif // GAMMARAY_PROPERTYEDITORDELEGATE_H
