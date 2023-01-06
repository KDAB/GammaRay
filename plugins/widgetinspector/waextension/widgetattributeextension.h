/*
  widgetattributeextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETATTRIBUTEEXTENSION_H
#define GAMMARAY_WIDGETATTRIBUTEEXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace GammaRay {
template<typename, typename>
class AttributeModel;

class WidgetAttributeExtension : public PropertyControllerExtension
{
public:
    explicit WidgetAttributeExtension(PropertyController *controller);
    ~WidgetAttributeExtension();

    bool setQObject(QObject *object) override;

private:
    AttributeModel<QWidget, Qt::WidgetAttribute> *m_attributeModel;
};
}

#endif // GAMMARAY_WIDGETATTRIBUTEEXTENSION_H
