/*
  bindingextension.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_BINDINGEXTENSION_H
#define GAMMARAY_BINDINGEXTENSION_H

// Own
#include <core/propertycontrollerextension.h>
#include "gammaray_core_export.h"

// Qt
#include <QObject>
#include <QPointer>

// Std
#include <memory>
#include <vector>

namespace GammaRay {

class BindingModel;
class BindingNode;

class GAMMARAY_CORE_EXPORT BindingExtension : public QObject, public PropertyControllerExtension
{
    Q_OBJECT
public:
    explicit BindingExtension(PropertyController *controller);
    ~BindingExtension() override;

    bool setQObject(QObject *object) override;

    BindingModel *model() const;

private slots:
    void propertyChanged();
    void clear();

private:
    QPointer<QObject> m_object;
    std::vector<std::unique_ptr<BindingNode>> m_bindings;

    BindingModel *m_bindingModel;
};
}

#endif // GAMMARAY_BINDINGEXTENSION_H
