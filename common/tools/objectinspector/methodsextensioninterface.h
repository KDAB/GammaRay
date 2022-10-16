/*
  methodsextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_METHODSEXTENSIONINTERFACE_H
#define GAMMARAY_METHODSEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {
/** @brief Client/Server interface of the methods viewer. */
class MethodsExtensionInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasObject READ hasObject WRITE setHasObject NOTIFY hasObjectChanged)
public:
    explicit MethodsExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~MethodsExtensionInterface() override;

    const QString &name() const;

    bool hasObject() const;
    void setHasObject(bool hasObject);

signals:
    void hasObjectChanged();

public slots:
    virtual void activateMethod() = 0;
    virtual void invokeMethod(Qt::ConnectionType type) = 0;
    virtual void connectToSignal() = 0;

private:
    QString m_name;
    bool m_hasObject;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::MethodsExtensionInterface,
                    "com.kdab.GammaRay.MethodsExtensionInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_METHODSEXTENSIONINTERFACE_H
