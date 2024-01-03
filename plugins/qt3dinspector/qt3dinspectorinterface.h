/*
  qt3dinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
