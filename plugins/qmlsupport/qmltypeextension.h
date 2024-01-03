/*
  qmltypeextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLTYPEEXTENSION_H
#define GAMMARAY_QMLTYPEEXTENSION_H

#include <core/propertycontrollerextension.h>

namespace GammaRay {
class AggregatedPropertyModel;

class QmlTypeExtension : public PropertyControllerExtension
{
public:
    explicit QmlTypeExtension(PropertyController *controller);
    ~QmlTypeExtension();

    bool setQObject(QObject *object) override;
    bool setMetaObject(const QMetaObject *metaObject) override;

private:
    AggregatedPropertyModel *m_typePropertyModel;
};
}

#endif // GAMMARAY_QMLTYPEEXTENSION_H
