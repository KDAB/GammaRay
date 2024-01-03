/*
  qtivipropertyoverrider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
