/*
  metatypebrowserinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
