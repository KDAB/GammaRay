/*
  util.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "util.h"
#include "common/classesiconsrepository.h"
#include "common/metatypedeclarations.h"
#include "common/objectbroker.h"
#include "varianthandler.h"
#include "objectdataprovider.h"
#include "enumutil.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QMetaObject>
#include <QObject>
#include <QPainter>

#include <private/qobject_p.h>
#include <private/qmetaobject_p.h>

#include <iostream>

using namespace GammaRay;
using namespace std;

QString Util::displayString(const QObject *object)
{
    if (!object)
        return QStringLiteral("0x0 (QObject)");
    const auto name = ObjectDataProvider::name(object);
    if (name.isEmpty())
        return QStringLiteral("%1 (%2)").arg(addressToString(object), object->metaObject()->className());
    const auto typeName = ObjectDataProvider::typeName(object);
    if (name == typeName)
        return name;
    return QStringLiteral("%1 (%2)").arg(ObjectDataProvider::typeName(object), name);
}

QString Util::shortDisplayString(const QObject *object)
{
    if (!object)
        return QStringLiteral("0x0");
    const auto name = ObjectDataProvider::name(object);
    if (name.isEmpty())
        return addressToString(object);
    return name;
}

QString Util::enumToString(const QVariant &value, const char *typeName, const QObject *object)
{
    return EnumUtil::enumToString(value, typeName, object ? object->metaObject() : nullptr);
}

QString Util::prettyMethodSignature(const QMetaMethod &method)
{
    QString signature = method.typeName();
    signature += ' ' + method.name() + '(';
    QStringList args;
    args.reserve(method.parameterCount());
    const QList<QByteArray> paramTypes = method.parameterTypes();
    const QList<QByteArray> paramNames = method.parameterNames();
    for (int i = 0; i < method.parameterCount(); ++i) {
        QString arg = paramTypes.at(i);
        if (!paramNames.at(i).isEmpty())
            arg += ' ' + paramNames.at(i);
        args.push_back(arg);
    }
    signature += args.join(QStringLiteral(", ")) + ')';
    return signature;
}

bool Util::descendantOf(const QObject *ascendant, const QObject *object)
{
    QObject *parent = object->parent();
    if (!parent)
        return false;
    if (parent == ascendant)
        return true;
    return descendantOf(ascendant, parent);
}

namespace GammaRay {
static QString stringifyProperty(const QObject *obj, const QString &propName)
{
    const QVariant value = obj->property(propName.toLatin1());
    const QMetaProperty mp = obj->metaObject()->property(
        obj->metaObject()->indexOfProperty(propName.toLatin1()));
    if (mp.isValid()) {
        const QString enumStr = EnumUtil::enumToString(value, mp.typeName(), obj->metaObject());
        if (!enumStr.isEmpty())
            return enumStr;
    }
    return VariantHandler::displayString(value);
}

struct IconCacheEntry
{
    explicit IconCacheEntry(const QByteArray &className_ = QByteArray())
        : className(className_)
    {
    }

    /// note: this member's data is being referenced by the database's key
    QByteArray className;
    int defaultIcon = -1;

    // pair of property name and expected string value
    typedef QPair<QString, QString> PropertyPair;
    // a list of property pairs
    using PropertyMap = QVector<PropertyPair>;
    // pair of icon and property map, for which this icon is valid
    typedef QPair<int, PropertyMap> PropertyIcon;
    using PropertyIcons = QVector<PropertyIcon>;
    PropertyIcons propertyIcons;
};

/// maps latin1 class name to list of icons valid for a given property map
/// for performance reasons, the key is just a view of the contents of IconCacheEntry::className
typedef QHash<QLatin1String, IconCacheEntry> IconDatabase;

static IconDatabase readIconData()
{
    auto classesIconsRepository = ObjectBroker::object<ClassesIconsRepository *>();
    IconDatabase data;
    int id = 0;

    for (auto it = classesIconsRepository->constBegin(), end = classesIconsRepository->constEnd();
         it != end; ++id, ++it) {
        const QFileInfo fileInfo((*it));
        const QDir dir(fileInfo.path());
        const QString className = dir.dirName();
        const QString iconName = fileInfo.fileName();

        const QByteArray classNameAsByteArray = className.toLatin1();
        const QLatin1String classNameKey(classNameAsByteArray.data(), classNameAsByteArray.size());

        auto dataIt = data.find(classNameKey);
        if (dataIt == data.end()) {
            dataIt = data.insert(classNameKey, IconCacheEntry(classNameAsByteArray));
        }

        IconCacheEntry &perClassData = *dataIt;
        if (iconName == QLatin1String("default.png")) {
            perClassData.defaultIcon = id;
        } else {
            // special property-specific icons with file name format prop1=val;prop2=val.png
            QString propString(iconName);
            propString.chop(4);
            const QStringList props = propString.split(';');
            IconCacheEntry::PropertyMap propertyMap;
            for (const QString &prop : std::as_const(props)) {
                const QStringList keyValue = prop.split(QLatin1Char('='));
                if (keyValue.size() != 2)
                    continue;
                propertyMap << qMakePair(keyValue.at(0), keyValue.at(1));
            }
            Q_ASSERT(!propertyMap.isEmpty());
            perClassData.propertyIcons << qMakePair(id, propertyMap);
        }
    }

    return data;
}

static int iconIdForObject(const QMetaObject *mo, const QObject *obj)
{
    static const IconDatabase iconDataBase = readIconData();

    auto it = iconDataBase.constFind(QLatin1String(mo->className()));
    if (it != iconDataBase.end()) {
        for (const auto &propertyIcon : std::as_const(it->propertyIcons)) {
            bool allMatch = true;
            Q_ASSERT(!propertyIcon.second.isEmpty());
            for (const IconCacheEntry::PropertyPair &keyValue : std::as_const(propertyIcon.second)) {
                if (stringifyProperty(obj, keyValue.first) != keyValue.second) {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch)
                return propertyIcon.first;
        }
        return it->defaultIcon;
    }

    if (mo->superClass())
        return iconIdForObject(mo->superClass(), obj);

    return -1;
}
}

int Util::iconIdForObject(const QObject *object)
{
    if (object)
        return GammaRay::iconIdForObject(object->metaObject(), object);
    return -1;
}

QString Util::tooltipForObject(const QObject *object)
{
    static const auto tip = qApp->translate("GammaRay::Util",
                                            "<p style='white-space:pre'>Object name: %1 (Address: %2)\nType: %3\nParent: %4 (Address: %5)\nNumber of children: %6</p>");

    return tip.arg(
        object->objectName().isEmpty() ? QStringLiteral("&lt;Not set&gt;") : object->objectName(),
        Util::addressToString(object),
        object->metaObject()->className(),
        object->parent() ? object->parent()->metaObject()->className() : QStringLiteral("<No parent>"),
        Util::addressToString(object->parent()),
        QString::number(object->children().size()));
}

void Util::drawTransparencyPattern(QPainter *painter, const QRect &rect, int squareSize)
{
    QPixmap bgPattern(2 * squareSize, 2 * squareSize);
    bgPattern.fill(Qt::lightGray);
    QPainter bgPainter(&bgPattern);
    bgPainter.fillRect(squareSize, 0, squareSize, squareSize, Qt::gray);
    bgPainter.fillRect(0, squareSize, squareSize, squareSize, Qt::gray);

    QBrush bgBrush;
    bgBrush.setTexture(bgPattern);
    painter->fillRect(rect, bgBrush);
}

int Util::signalIndexToMethodIndex(const QMetaObject *metaObject, int signalIndex)
{
    return QMetaObjectPrivate::signal(metaObject, signalIndex).methodIndex();
}
