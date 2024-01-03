/*
  propertyeditorfactory.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYEDITORFACTORY_H
#define GAMMARAY_PROPERTYEDITORFACTORY_H

#include "gammaray_ui_export.h"

#include <QItemEditorFactory>
#include <QVector>

namespace GammaRay {
/** Item editor factory with support for extra types while keeping support for the built-in ones. */
class GAMMARAY_UI_EXPORT PropertyEditorFactory : public QItemEditorFactory
{
public:
    static PropertyEditorFactory *instance();

    typedef int TypeId;

    QWidget *createEditor(TypeId type, QWidget *parent) const override;

    static QVector<int> supportedTypes();
    static bool hasExtendedEditor(int typeId);

protected:
    PropertyEditorFactory();

private:
    Q_DISABLE_COPY(PropertyEditorFactory)
    void initBuiltInTypes();
    void addEditor(TypeId type, QItemEditorCreatorBase *creator, bool extended = false);

    QVector<int> m_supportedTypes;
    QVector<int> m_extendedTypes;
};
}

#endif // GAMMARAY_PROPERTYEDITORFACTORY_H
