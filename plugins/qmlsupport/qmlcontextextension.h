/*
  qmlcontextextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLCONTEXTEXTENSION_H
#define GAMMARAY_QMLCONTEXTEXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class AggregatedPropertyModel;
class QmlContextModel;

class QmlContextExtension : public PropertyControllerExtension
{
public:
    explicit QmlContextExtension(PropertyController *controller);
    ~QmlContextExtension();

    bool setQObject(QObject *object) override;

private:
    void contextSelected(const QItemSelection &selection);

    QmlContextModel *m_contextModel;
    AggregatedPropertyModel *m_propertyModel;
};
}

#endif // GAMMARAY_QMLCONTEXTEXTENSION_H
