/*
  propertytest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QCoreApplication>
#include <QTimer>

#include "../shared/propertytestobject.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    PropertyTestObject obj;
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, obj.changingPropertyObject(),
                     &ChangingPropertyObject::changeProperties);
    timer.start(5000);

    return app.exec();
}
