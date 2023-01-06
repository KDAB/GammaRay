/*
  qtiviconstrainedvaluedelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
