/*
  qtiviconstrainedvaluedelegate.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
