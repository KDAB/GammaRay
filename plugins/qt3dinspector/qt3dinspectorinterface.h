/*
  qt3dinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QT3DINSPECTORINTERFACE_H
#define GAMMARAY_QT3DINSPECTORINTERFACE_H

#include <QObject>

namespace GammaRay {
class Qt3DInspectorInterface : public QObject
{
    Q_OBJECT
public:
    explicit Qt3DInspectorInterface(QObject *parent = nullptr);
    ~Qt3DInspectorInterface();

public slots:
    virtual void selectEngine(int index) = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::Qt3DInspectorInterface,
                    "com.kdab.GammaRay.Qt3DInspectorInterface/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_QT3DINSPECTORINTERFACE_H
