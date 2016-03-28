/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qmlsupport.h"
#include "qmllistpropertyadaptor.h"
#include "qmlattachedpropertyadaptor.h"
#include "qjsvaluepropertyadaptor.h"
#include "qmltypeextension.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>
#include <core/util.h>
#include <core/propertyadaptorfactory.h>
#include <core/propertycontroller.h>
#include <core/objectdataprovider.h>

#include <common/metatypedeclarations.h>
#include <common/sourcelocation.h>

#include <QDateTime>
#include <QDebug>
#include <QJSValue>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlError>
#include <QQmlListProperty>

#include <private/qqmlmetatype_p.h>
#include <private/qqmldata_p.h>
#include <private/qqmlcontext_p.h>

Q_DECLARE_METATYPE(QQmlError)

using namespace GammaRay;

static QString qmlErrorToString(const QQmlError &error)
{
  return QStringLiteral("%1:%2:%3: %4")
    .arg(error.url().toString())
    .arg(error.line())
    .arg(error.column())
    .arg(error.description());
}

static QString qmlListPropertyToString(const QVariant &value, bool *ok)
{
  if (qstrncmp(value.typeName(), "QQmlListProperty<", 17) != 0 || !value.isValid())
    return QString();

  *ok = true;
  QQmlListProperty<QObject> *prop = reinterpret_cast<QQmlListProperty<QObject>*>(const_cast<void*>(value.data()));
  const int count = prop->count(prop);
  if (!count)
    return QmlSupport::tr("<empty>");
  return QmlSupport::tr("<%1 entries>").arg(count);
}

static QString qjsValueToString(const QJSValue &v)
{
  if (v.isArray()) {
    return QStringLiteral("<array>");
  } else if (v.isBool()) {
    return v.toBool() ? QStringLiteral("true") : QStringLiteral("false");
  } else if (v.isCallable()) {
    return QStringLiteral("<callable>");
  } else if (v.isDate()) {
    return v.toDateTime().toString();
  } else if (v.isError()) {
    return QStringLiteral("<error>");
  } else if (v.isNull()) {
    return QStringLiteral("<null>");
  } else if (v.isNumber()) {
    return QString::number(v.toNumber());
  } else if (v.isObject()) {
    return QStringLiteral("<object>");
  } else if (v.isQObject()) {
    return Util::displayString(v.toQObject());
  } else if (v.isRegExp()) {
    return QStringLiteral("<regexp>");
  } else if (v.isString()) {
    return v.toString();
  } else if (v.isUndefined()) {
    return QStringLiteral("<undefined>");
  } else if (v.isVariant()) {
    return VariantHandler::displayString(v.toVariant());
  }
  return QStringLiteral("<unknown QJSValue>");
}

namespace GammaRay {
class QmlObjectDataProvider : public AbstractObjectDataProvider
{
public:
    QString name(const QObject* obj) const Q_DECL_OVERRIDE;
    SourceLocation creationLocation(QObject* obj) const Q_DECL_OVERRIDE;
};
}

QString QmlObjectDataProvider::name(const QObject *obj) const
{
    QQmlContext *ctx = QQmlEngine::contextForObject(obj);
    const auto id = ctx ? ctx->nameForObject(const_cast<QObject*>(obj)) : QString();
    return id;
}

SourceLocation QmlObjectDataProvider::creationLocation(QObject *obj) const
{
    SourceLocation loc;

    auto objectData = QQmlData::get(obj);
    if (!objectData)
        return loc;

    auto context = objectData->outerContext;
    if (!context)
        return loc;

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    loc.setFileName(context->url().scheme() == QStringLiteral("file")
            ? context->url().path()
            : context->url().toString() // Most editors don't understand paths with the file://
                                         // scheme, still we need the scheme for anything else
                                         // but file (e.g. qrc:/)
        );
#else
    loc.setFileName(context->url.scheme() == QStringLiteral("file")
            ? context->url.path()
            : context->url.toString() // same as above
        );
#endif

    loc.setLine(objectData->lineNumber);
    loc.setColumn(objectData->columnNumber);
    return loc;
}

QmlSupport::QmlSupport(GammaRay::ProbeInterface* probe, QObject* parent) :
  QObject(parent)
{
  Q_UNUSED(probe);

  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QQmlComponent, QObject);
  MO_ADD_PROPERTY_RO(QQmlComponent, QList<QQmlError>, errors);
  MO_ADD_PROPERTY_RO(QQmlComponent, bool, isError);
  MO_ADD_PROPERTY_RO(QQmlComponent, bool, isLoading);
  MO_ADD_PROPERTY_RO(QQmlComponent, bool, isNull);
  MO_ADD_PROPERTY_RO(QQmlComponent, bool, isReady);

  MO_ADD_METAOBJECT1(QQmlContext, QObject);
  MO_ADD_PROPERTY_CR(QQmlContext, QUrl, baseUrl, setBaseUrl);
  MO_ADD_PROPERTY   (QQmlContext, QObject*, contextObject, setContextObject);
  MO_ADD_PROPERTY_RO(QQmlContext, QQmlEngine*, engine);
  MO_ADD_PROPERTY_RO(QQmlContext, bool, isValid);
  MO_ADD_PROPERTY_RO(QQmlContext, QQmlContext*, parentContext);

  MO_ADD_METAOBJECT1(QQmlEngine, QObject);
  MO_ADD_PROPERTY_CR(QQmlEngine, QUrl, baseUrl, setBaseUrl);
  MO_ADD_PROPERTY_CR(QQmlEngine, QStringList, importPathList, setImportPathList);
  MO_ADD_PROPERTY   (QQmlEngine, bool, outputWarningsToStandardError, setOutputWarningsToStandardError);
  MO_ADD_PROPERTY_CR(QQmlEngine, QStringList, pluginPathList, setPluginPathList);
  MO_ADD_PROPERTY_RO(QQmlEngine, QQmlContext*, rootContext);

  MO_ADD_METAOBJECT0(QQmlType);
  MO_ADD_PROPERTY_RO(QQmlType, QByteArray, typeName);
  MO_ADD_PROPERTY_RO(QQmlType, const QString&, qmlTypeName);
  MO_ADD_PROPERTY_RO(QQmlType, const QString&, elementName);
  MO_ADD_PROPERTY_RO(QQmlType, int, majorVersion);
  MO_ADD_PROPERTY_RO(QQmlType, int, minorVersion);
  MO_ADD_PROPERTY_RO(QQmlType, int, createSize);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isCreatable);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isExtendedType);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isSingleton);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isInterface);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isComposite);
  MO_ADD_PROPERTY_RO(QQmlType, bool, isCompositeSingleton);
  MO_ADD_PROPERTY_RO(QQmlType, QString, noCreationReason);
  MO_ADD_PROPERTY_RO(QQmlType, int, typeId);
  MO_ADD_PROPERTY_RO(QQmlType, int, qListTypeId);
  MO_ADD_PROPERTY_RO(QQmlType, int, metaObjectRevision);
  MO_ADD_PROPERTY_RO(QQmlType, bool, containsRevisionedAttributes);
//   MO_ADD_PROPERTY_RO(QQmlType, const char*, interfaceIId);
  MO_ADD_PROPERTY_RO(QQmlType, int, index);
  MO_ADD_PROPERTY_RO(QQmlType, const QMetaObject*, metaObject);
  MO_ADD_PROPERTY_RO(QQmlType, const QMetaObject*, baseMetaObject);
  MO_ADD_PROPERTY_RO(QQmlType, QUrl, sourceUrl);

  VariantHandler::registerStringConverter<QJSValue>(qjsValueToString);
  VariantHandler::registerStringConverter<QQmlError>(qmlErrorToString);
  VariantHandler::registerGenericStringConverter(qmlListPropertyToString);

  PropertyAdaptorFactory::registerFactory(QmlListPropertyAdaptorFactory::instance());
  PropertyAdaptorFactory::registerFactory(QmlAttachedPropertyAdaptorFactory::instance());
  PropertyAdaptorFactory::registerFactory(QJSValuePropertyAdaptorFactory::instance());

  PropertyController::registerExtension<QmlTypeExtension>();

  static auto dataProvider = new QmlObjectDataProvider;
  ObjectDataProvider::registerProvider(dataProvider);
}

QString QmlSupportFactory::name() const
{
  return tr("QML Support");
}
