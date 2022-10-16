/*
  materialextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSIONINTERFACE_H
#define GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {
/** @brief Client/Server interface of the material viewer. */
class MaterialExtensionInterface : public QObject
{
    Q_OBJECT
public:
    explicit MaterialExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~MaterialExtensionInterface() override;

    const QString &name() const;

signals:
    void gotShader(const QString &shaderSource);

public slots:
    virtual void getShader(int row) = 0;

private:
    QString m_name;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::MaterialExtensionInterface,
                    "com.kdab.GammaRay.MaterialExtensionInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_MATERIALEXTENSIONINTERFACE_H
