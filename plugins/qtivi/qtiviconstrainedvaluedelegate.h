/*
  qtiviconstrainedvaluedelegate.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef GAMMARAY_QTIVICONSTRAINEDVALUEDELEGATE_H
#define GAMMARAY_QTIVICONSTRAINEDVALUEDELEGATE_H

#include <ui/propertyeditor/propertyeditordelegate.h>

namespace GammaRay {

class QtIviConstrainedValueDelegate : public PropertyEditorDelegate
{
    Q_OBJECT
public:
    explicit QtIviConstrainedValueDelegate(QObject *parent);
    ~QtIviConstrainedValueDelegate() override;

    // Override or modify the editor for certain cases:
    // - Set limits in spinboxes for numeric ranges
    // - Show a special combobox for properties which have a list of allowed values, e.g.
    //   QFlags with only some allowed combinations, like AirflowDirections
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

}

#endif // GAMMARAY_QTIVICONSTRAINEDVALUEDELEGATE_H
