/*
  metaobjectrepository.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "metaobjectrepository.h"
#include "metaobject.h"

#include <common/metatypedeclarations.h>

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QDateTime>
#include <QEasingCurve>
#include <QFile>
#include <QObject>
#include <private/qobject_p.h>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QThread>
#include <QTimer>

#include <QSaveFile>
#include <QTimeZone>

using namespace GammaRay;

namespace GammaRay {
class StaticMetaObjectRepository : public MetaObjectRepository
{
public:
    StaticMetaObjectRepository() = default;
};
}

Q_GLOBAL_STATIC(StaticMetaObjectRepository, s_instance)

MetaObjectRepository::~MetaObjectRepository()
{
    qDeleteAll(m_metaObjects);
}

void MetaObjectRepository::initBuiltInTypes()
{
    m_initialized = true;
    initQObjectTypes();
    initIOTypes();
    initQEventTypes();
}

Q_DECLARE_METATYPE(QThread::Priority)

void MetaObjectRepository::initQObjectTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QMetaObject);
    MO_ADD_PROPERTY_RO(QMetaObject, classInfoCount);
    MO_ADD_PROPERTY_RO(QMetaObject, classInfoOffset);
// MO_ADD_PROPERTY_RO(QMetaObject, className);
    MO_ADD_PROPERTY_RO(QMetaObject, constructorCount);
    MO_ADD_PROPERTY_RO(QMetaObject, enumeratorCount);
    MO_ADD_PROPERTY_RO(QMetaObject, enumeratorOffset);
    MO_ADD_PROPERTY_RO(QMetaObject, methodCount);
    MO_ADD_PROPERTY_RO(QMetaObject, methodOffset);
    MO_ADD_PROPERTY_RO(QMetaObject, propertyCount);
    MO_ADD_PROPERTY_RO(QMetaObject, propertyOffset);
    MO_ADD_PROPERTY_RO(QMetaObject, superClass);

    MO_ADD_METAOBJECT0(QObject);
    MO_ADD_PROPERTY_RO(QObject, metaObject);
    MO_ADD_PROPERTY_RO(QObject, parent);
    MO_ADD_PROPERTY_RO(QObject, signalsBlocked); // TODO setter has non-void return type
    MO_ADD_PROPERTY_RO(QObject, thread);

    MO_ADD_METAOBJECT1(QThread, QObject)
    MO_ADD_PROPERTY_RO(QThread, isFinished);
    MO_ADD_PROPERTY_RO(QThread, isInterruptionRequested);
    MO_ADD_PROPERTY_RO(QThread, isRunning);
    MO_ADD_PROPERTY(QThread, priority, setPriority);
    MO_ADD_PROPERTY(QThread, stackSize, setStackSize);

    MO_ADD_METAOBJECT1(QTimer, QObject);
    MO_ADD_PROPERTY_RO(QTimer, timerId);

    MO_ADD_METAOBJECT1(QCoreApplication, QObject);
    MO_ADD_PROPERTY_ST(QCoreApplication, applicationDirPath);
    MO_ADD_PROPERTY_ST(QCoreApplication, applicationFilePath);
    MO_ADD_PROPERTY_ST(QCoreApplication, applicationPid);
    MO_ADD_PROPERTY_ST(QCoreApplication, arguments);
    MO_ADD_PROPERTY_ST(QCoreApplication, closingDown);
    MO_ADD_PROPERTY_ST(QCoreApplication, isQuitLockEnabled);
    MO_ADD_PROPERTY_ST(QCoreApplication, isSetuidAllowed);
    MO_ADD_PROPERTY_ST(QCoreApplication, libraryPaths);
    MO_ADD_PROPERTY_ST(QCoreApplication, startingUp);

    MO_ADD_METAOBJECT1(QAbstractItemModel, QObject);
    MO_ADD_PROPERTY_RO(QAbstractItemModel, mimeTypes);
    MO_ADD_PROPERTY_RO(QAbstractItemModel, roleNames);
    MO_ADD_PROPERTY_RO(QAbstractItemModel, supportedDragActions);
    MO_ADD_PROPERTY_RO(QAbstractItemModel, supportedDropActions);
    MO_ADD_METAOBJECT1(QAbstractProxyModel, QAbstractItemModel);
    MO_ADD_METAOBJECT1(QSortFilterProxyModel, QAbstractProxyModel);
    MO_ADD_PROPERTY_RO(QSortFilterProxyModel, sortOrder);

    MO_ADD_METAOBJECT0(QDateTime);
    MO_ADD_PROPERTY_RO(QDateTime, isDaylightTime);
    MO_ADD_PROPERTY_RO(QDateTime, isNull);
    MO_ADD_PROPERTY_RO(QDateTime, isValid);
    MO_ADD_PROPERTY_RO(QDateTime, offsetFromUtc);
    MO_ADD_PROPERTY(QDateTime, timeZone, setTimeZone);

    MO_ADD_METAOBJECT0(QTimeZone);
    MO_ADD_PROPERTY_RO(QTimeZone, comment);
    MO_ADD_PROPERTY_RO(QTimeZone, country);
    MO_ADD_PROPERTY_RO(QTimeZone, hasDaylightTime);
    MO_ADD_PROPERTY_RO(QTimeZone, hasTransitions);
    MO_ADD_PROPERTY_RO(QTimeZone, id);
    MO_ADD_PROPERTY_RO(QTimeZone, isValid);

    MO_ADD_METAOBJECT0(QEasingCurve);
    MO_ADD_PROPERTY(QEasingCurve, amplitude, setAmplitude);
    MO_ADD_PROPERTY(QEasingCurve, overshoot, setOvershoot);
    MO_ADD_PROPERTY(QEasingCurve, period, setPeriod);
    MO_ADD_PROPERTY(QEasingCurve, type, setType);
}

Q_DECLARE_METATYPE(QIODevice::OpenMode)
Q_DECLARE_METATYPE(QFileDevice::FileError)
Q_DECLARE_METATYPE(QFileDevice::Permissions)

void MetaObjectRepository::initIOTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QIODevice, QObject);
    MO_ADD_PROPERTY_RO(QIODevice, openMode);
    MO_ADD_PROPERTY(QIODevice, isTextModeEnabled, setTextModeEnabled);
    MO_ADD_PROPERTY_RO(QIODevice, isOpen);
    MO_ADD_PROPERTY_RO(QIODevice, isReadable);
    MO_ADD_PROPERTY_RO(QIODevice, isWritable);
    MO_ADD_PROPERTY_RO(QIODevice, isSequential);
    MO_ADD_PROPERTY_RO(QIODevice, pos);
    MO_ADD_PROPERTY_RO(QIODevice, size);
    MO_ADD_PROPERTY_RO(QIODevice, atEnd);
    MO_ADD_PROPERTY_RO(QIODevice, bytesAvailable);
    MO_ADD_PROPERTY_RO(QIODevice, bytesToWrite);
    MO_ADD_PROPERTY_RO(QIODevice, canReadLine);
    MO_ADD_PROPERTY_RO(QIODevice, errorString);

    // FIXME: QIODevice::readAll() would be nice to have

    MO_ADD_METAOBJECT1(QFileDevice, QIODevice);
    MO_ADD_PROPERTY_RO(QFileDevice, error);
    MO_ADD_PROPERTY_RO(QFileDevice, fileName);
    MO_ADD_PROPERTY_RO(QFileDevice, handle);
    MO_ADD_PROPERTY_RO(QFileDevice, permissions);

    MO_ADD_METAOBJECT1(QFile, QFileDevice);
    MO_ADD_PROPERTY_RO(QFile, exists);
    MO_ADD_PROPERTY_RO(QFile, symLinkTarget);

    MO_ADD_METAOBJECT1(QSaveFile, QFileDevice);
}

Q_DECLARE_METATYPE(const QObject*)

void MetaObjectRepository::initQEventTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QEvent);
    MO_ADD_PROPERTY_RO(QEvent, isAccepted);
    MO_ADD_PROPERTY_RO(QEvent, spontaneous);
    MO_ADD_PROPERTY_RO(QEvent, type);

    MO_ADD_METAOBJECT1(QTimerEvent, QEvent);
    MO_ADD_PROPERTY_RO(QTimerEvent, timerId);

    MO_ADD_METAOBJECT1(QChildEvent, QEvent);
    MO_ADD_PROPERTY_RO(QChildEvent, child);
    MO_ADD_PROPERTY_RO(QChildEvent, added);
    MO_ADD_PROPERTY_RO(QChildEvent, polished);
    MO_ADD_PROPERTY_RO(QChildEvent, removed);

    MO_ADD_METAOBJECT1(QDynamicPropertyChangeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QDynamicPropertyChangeEvent, propertyName);

    MO_ADD_METAOBJECT1(QDeferredDeleteEvent, QEvent);
    MO_ADD_PROPERTY_RO(QDeferredDeleteEvent, loopLevel);

    MO_ADD_METAOBJECT1(QMetaCallEvent, QEvent);
    MO_ADD_PROPERTY_RO(QMetaCallEvent, id);
    MO_ADD_PROPERTY_RO(QMetaCallEvent, sender);  // problematic because type is const QObject*
    MO_ADD_PROPERTY_RO(QMetaCallEvent, signalId);
}

MetaObjectRepository *MetaObjectRepository::instance()
{
    if (!s_instance()->m_initialized)
        s_instance()->initBuiltInTypes();
    return s_instance();
}

void MetaObjectRepository::addMetaObject(MetaObject *mo)
{
    Q_ASSERT(!mo->className().isEmpty());
    Q_ASSERT(!m_metaObjects.contains(mo->className()));
    m_metaObjects.insert(mo->className(), mo);
    int idx = 0;
    while (auto super = mo->superClass(idx++))
        m_derivedTypes[super].push_back(mo);
}

MetaObject *MetaObjectRepository::metaObject(const QString &typeName) const
{
    QString typeName_ = typeName;
    typeName_.remove('*');
    typeName_.remove('&');
    typeName_.remove(QStringLiteral("const "));
    typeName_.remove(QStringLiteral(" const"));
    typeName_.remove(' ');
    return m_metaObjects.value(typeName_);
}

MetaObject* MetaObjectRepository::metaObject(const QString& typeName, void *&obj) const
{
    auto mo = metaObject(typeName);
    return metaObject(mo, obj);
}

MetaObject* MetaObjectRepository::metaObject(MetaObject *mo, void *&obj) const
{
    if (!mo || !mo->isPolymorphic())
        return mo;
    const auto derivedIt = m_derivedTypes.find(mo);
    if (derivedIt == m_derivedTypes.end())
        return mo;
    for (auto derivedMo : (*derivedIt).second) {
        auto childObj = derivedMo->castFrom(obj, mo);
        if (!childObj)
            continue;
        obj = childObj;
        return metaObject(derivedMo, obj);
    }
    return mo;
}

bool MetaObjectRepository::hasMetaObject(const QString &typeName) const
{
    return m_metaObjects.contains(typeName);
}

void MetaObjectRepository::clear()
{
    qDeleteAll(m_metaObjects);
    m_metaObjects.clear();
    m_initialized = false;
}
