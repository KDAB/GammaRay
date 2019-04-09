/*
  propertyeditorfactory.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
