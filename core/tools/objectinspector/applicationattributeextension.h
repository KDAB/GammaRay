/*
  applicationattributeextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_APPLICATIONATTRIBUTEEXTENSION_H
#define GAMMARAY_APPLICATIONATTRIBUTEEXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QCoreApplication;
QT_END_NAMESPACE

namespace GammaRay {
template<typename, typename>
class AttributeModel;

class ApplicationAttributeExtension : public PropertyControllerExtension
{
public:
    explicit ApplicationAttributeExtension(PropertyController *controller);
    ~ApplicationAttributeExtension();

    bool setQObject(QObject *object) override;

private:
    AttributeModel<QCoreApplication, Qt::ApplicationAttribute> *m_attributeModel;
};
}

#endif // GAMMARAY_APPLICATIONATTRIBUTEEXTENSION_H
