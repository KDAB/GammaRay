/*
  enumsextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_ENUMSEXTENSION_H
#define GAMMARAY_OBJECTINSPECTOR_ENUMSEXTENSION_H

#include "propertycontrollerextension.h"

namespace GammaRay {
class PropertyController;
class ObjectEnumModel;

class EnumsExtension : public PropertyControllerExtension
{
public:
    explicit EnumsExtension(PropertyController *controller);
    ~EnumsExtension();

    bool setQObject(QObject *object) override;
    bool setMetaObject(const QMetaObject *metaObject) override;

private:
    ObjectEnumModel *m_model;
};
}

#endif // ENUMSEXTENSION_H
