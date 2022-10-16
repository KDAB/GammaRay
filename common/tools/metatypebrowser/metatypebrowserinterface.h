/*
  metatypebrowserinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METATYPEBROWSERINTERFACE_H
#define GAMMARAY_METATYPEBROWSERINTERFACE_H

#include <QObject>

namespace GammaRay {

/*! communication interface for the meta type browser tool. */
class MetaTypeBrowserInterface : public QObject
{
    Q_OBJECT
public:
    explicit MetaTypeBrowserInterface(QObject *parent = nullptr);
    ~MetaTypeBrowserInterface() override;

public slots:
    virtual void rescanTypes() = 0;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::MetaTypeBrowserInterface, "com.kdab.GammaRay.MetaTypeBrowserInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_METATYPEBROWSERINTERFACE_H
