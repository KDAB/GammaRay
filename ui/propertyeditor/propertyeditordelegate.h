/*
  propertyeditordelegate.h

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

#ifndef GAMMARAY_PROPERTYEDITORDELEGATE_H
#define GAMMARAY_PROPERTYEDITORDELEGATE_H

#include "gammaray_ui_export.h"
#include <QStyledItemDelegate>

class QMatrix4x4;

namespace GammaRay {

class GAMMARAY_UI_EXPORT PropertyEditorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PropertyEditorDelegate(QObject *parent);
    ~PropertyEditorDelegate();

    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
    template <typename Matrix>
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index, const Matrix &matrix) const;
    template <typename Matrix>
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex &index, const Matrix &matrix) const;
    template <typename Matrix>
    int columnWidth(const QStyleOptionViewItem& option, const Matrix& matrix, int column) const;
};

}

#endif // GAMMARAY_PROPERTYEDITORDELEGATE_H
