/*
  qmlsupportuifactory.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qmlsupportuifactory.h"
#include "qmlcontexttab.h"
#include "qmltypetab.h"

#include <ui/propertywidget.h>

using namespace GammaRay;

QString QmlSupportUiFactory::id() const
{
    return QString();
}

void QmlSupportUiFactory::initUi()
{
    PropertyWidget::registerTab<QmlContextTab>(QStringLiteral("qmlContext"), tr("QML Context"),
                                               PropertyWidgetTabPriority::Advanced);
    PropertyWidget::registerTab<QmlTypeTab>(QStringLiteral("qmlType"), tr("QML Type"),
                                            PropertyWidgetTabPriority::Exotic);
}

QWidget *GammaRay::QmlSupportUiFactory::createWidget(QWidget *)
{
    return nullptr;
}
