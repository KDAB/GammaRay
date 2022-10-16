/*
  propertiesextensioninterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H
#define GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H

#include <QObject>

namespace GammaRay {
/** @brief Client/Server interface of the property editor. */
class PropertiesExtensionInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        bool canAddProperty READ canAddProperty WRITE setCanAddProperty NOTIFY canAddPropertyChanged)
    Q_PROPERTY(
        bool hasPropertyValues READ hasPropertyValues WRITE setHasPropertyValues NOTIFY hasPropertyValuesChanged)
public:
    explicit PropertiesExtensionInterface(const QString &name, QObject *parent = nullptr);
    ~PropertiesExtensionInterface() override;

    const QString &name() const;

    bool canAddProperty() const;
    void setCanAddProperty(bool canAdd);

    bool hasPropertyValues() const;
    void setHasPropertyValues(bool hasValues);

public slots:
    virtual void setProperty(const QString &name, const QVariant &value) = 0;

signals:
    void canAddPropertyChanged();
    void hasPropertyValuesChanged();

private:
    QString m_name;
    bool m_canAddProperty;
    bool m_hasPropertyValues;
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::PropertiesExtensionInterface,
                    "com.kdab.GammaRay.PropertiesExtensionInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_PROPERTIESEXTENSIONINTERFACE_H
