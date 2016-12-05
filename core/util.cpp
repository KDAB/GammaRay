/*
  util.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "util.h"
#include "common/metatypedeclarations.h"
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

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <private/qobject_p.h>
#include <private/qmetaobject_p.h>
#endif

#include <iostream>

using namespace GammaRay;
using namespace std;

QString Util::displayString(const QObject *object)
{
    if (!object)
        return QStringLiteral("QObject(0x0)");
    const auto name = ObjectDataProvider::name(object);
    if (name.isEmpty())
        return QStringLiteral("%1[this=%2]").arg(object->metaObject()->className(),
                                                 addressToString(object));
    return name;
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

QString Util::addressToString(const void *p)
{
    return QLatin1String("0x") + QString::number(reinterpret_cast<qlonglong>(p), 16);
}

QString Util::enumToString(const QVariant& value, const char* typeName, const QObject* object)
{
    return EnumUtil::enumToString(value, typeName, object ? object->metaObject() : nullptr);
}

QString Util::prettyMethodSignature(const QMetaMethod &method)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return method.signature();
#else
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
#endif
}


bool Util::descendantOf(const QObject *ascendant, const QObject *obj)
{
    QObject *parent = obj->parent();
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
    const QMetaProperty mp
        = obj->metaObject()->property(
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
    QVariant defaultIcon;

    // pair of property name and expected string value
    typedef QPair<QString, QString> PropertyPair;
    // a list of property pairs
    typedef QVector<PropertyPair> PropertyMap;
    // pair of icon and property map, for which this icon is valid
    typedef QPair<QVariant, PropertyMap> PropertyIcon;
    typedef QVector<PropertyIcon> PropertyIcons;
    PropertyIcons propertyIcons;
};
/// maps latin1 class name to list of icons valid for a given property map
typedef QHash<QByteArray, IconCacheEntry> IconDatabase;

static IconDatabase readIconData()
{
    IconDatabase data;
    // we can't load icons due to their QPixmap dependency in a QCoreApplication
    if (!qApp->inherits("QGuiApplication") && !qApp->inherits("QApplication"))
        return data;

    const QString basePath = QStringLiteral(":/gammaray/classes/");
    QDir dir(basePath);

    const QStringList filterList = QStringList() << QStringLiteral("*.png");

    foreach (const QFileInfo &classEntry, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        IconCacheEntry perClassData;
        dir.cd(classEntry.fileName());
        const QStringList classIcons = dir.entryList(filterList, QDir::Files);
        dir.cdUp();
        if (classIcons.isEmpty()) {
            cerr << "invalid class icon resource file: "
                 << qPrintable(classEntry.absoluteFilePath()) << endl;
            continue;
        }
        foreach (const QString &iconName, classIcons) {
            const QIcon icon(classEntry.absoluteFilePath() + '/' + iconName);
            if (iconName == QLatin1String("default.png")) {
                perClassData.defaultIcon = icon;
                continue;
            }
            // special property-specific icons with file name format prop1=val;prop2=val.png
            QString propString(iconName);
            propString.chop(4);
            const QStringList props = propString.split(';');
            IconCacheEntry::PropertyMap propertyMap;
            foreach (const QString &prop, props) {
                const QStringList keyValue = prop.split(QLatin1Char('='));
                if (keyValue.size() != 2)
                    continue;
                propertyMap << qMakePair(keyValue.at(0), keyValue.at(1));
            }
            Q_ASSERT(!propertyMap.isEmpty());
            perClassData.propertyIcons << qMakePair(QVariant::fromValue(icon), propertyMap);
        }
        data[classEntry.fileName().toLatin1()] = perClassData;
    }
    return data;
}

static QVariant iconForObject(const QMetaObject *mo, const QObject *obj)
{
    static const IconDatabase iconDataBase = readIconData();
    // stupid Qt convention to use int for sizes... the static cast shuts down warnings about conversion from size_t to int.
    const QByteArray className
        = QByteArray::fromRawData(mo->className(), static_cast<int>(strlen(mo->className())));
    IconDatabase::const_iterator it = iconDataBase.constFind(className);
    if (it != iconDataBase.constEnd()) {
        foreach (const IconCacheEntry::PropertyIcon &propertyIcon, it->propertyIcons) {
            bool allMatch = true;
            Q_ASSERT(!propertyIcon.second.isEmpty());
            foreach (const IconCacheEntry::PropertyPair &keyValue, propertyIcon.second) {
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
        return iconForObject(mo->superClass(), obj);

    return QVariant();
}
}

QVariant Util::iconForObject(const QObject *obj)
{
    if (obj)
        return GammaRay::iconForObject(obj->metaObject(), obj);
    return QVariant();
}

QString Util::tooltipForObject(const QObject *object)
{
    return QObject::tr(
        "<p style='white-space:pre'>Object name: %1 (Address: %2)\nType: %3\nParent: %4 (Address: %5)\nNumber of children: %6</p>")
           .arg(
        object->objectName().isEmpty() ? QStringLiteral("&lt;Not set&gt;") : object->objectName(),
        Util::addressToString(object),
        object->metaObject()->className(),
        object->parent() ? object->parent()->metaObject()->className() : QStringLiteral(
            "<No parent>"),
        Util::addressToString(object->parent()),
        QString::number(object->children().size())
        );
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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0) && defined(HAVE_PRIVATE_QT_HEADERS)
// from Qt4's qobject.cpp
static void computeOffsets(const QMetaObject *mo, int *signalOffset, int *methodOffset)
{
    *signalOffset = *methodOffset = 0;
    const QMetaObject *m = mo->superClass();
    while (m) {
        const QMetaObjectPrivate *d = QMetaObjectPrivate::get(m);
        *methodOffset += d->methodCount;
        *signalOffset += d->signalCount;
        m = m->superClass();
    }
}

#endif

int Util::signalIndexToMethodIndex(const QMetaObject *metaObject, int signalIndex)
{
#ifdef HAVE_PRIVATE_QT_HEADERS
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QMetaObjectPrivate::signal(metaObject, signalIndex).methodIndex();
#else

    if (signalIndex < 0)
        return signalIndex;
    Q_ASSERT(metaObject);

    int signalOffset, methodOffset;
    computeOffsets(metaObject, &signalOffset, &methodOffset);
    while (signalOffset > signalIndex) {
        metaObject = metaObject->superClass();
        computeOffsets(metaObject, &signalOffset, &methodOffset);
    }
    const int offset = methodOffset - signalOffset;
    return metaObject->method(signalIndex + offset).methodIndex();
#endif
#else
    Q_UNUSED(metaObject);
    Q_UNUSED(signalIndex);
    return -1;
#endif
}
