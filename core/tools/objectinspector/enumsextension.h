/*
  enumsextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
