/*
  qtivipropertyoverrider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QTIVIPROPERTYOVERRIDER_H
#define GAMMARAY_QTIVIPROPERTYOVERRIDER_H

#include <QObject>
#include <QPointer>
#include <QVariant>

class QIviProperty;

namespace GammaRay {

class QtIviPropertyOverrider
{
public:
    QtIviPropertyOverrider() = default;
    explicit QtIviPropertyOverrider(QIviProperty *property);
    explicit QtIviPropertyOverrider(QtIviPropertyOverrider &&other);

    QtIviPropertyOverrider &operator=(QtIviPropertyOverrider &&other);
    ~QtIviPropertyOverrider();

    void setValue(const QVariant &value);
    bool userWritable() const;
    void setOverride(bool);
    bool isOverride() const;

    QVariant cppValue() const;

private:
    Q_DISABLE_COPY(QtIviPropertyOverrider)
    friend class OverrideValueSetter;
    friend class OverrideValueGetter;

    QPointer<QIviProperty> m_prop;
    QtPrivate::QSlotObjectBase *m_originalValueGetter = nullptr;
    QtPrivate::QSlotObjectBase *m_originalValueSetter = nullptr;
    QVariant m_overrideValue;
};

}

#endif // GAMMARAY_QTIVIPROPERTYOVERRIDER_H
