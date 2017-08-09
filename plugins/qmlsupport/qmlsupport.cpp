/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "qmlcontextpropertyadaptor.h"
#include "qmlcontextextension.h"
#include "qmltypeextension.h"
#include "qmltypeutil.h"

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
#include <QQmlEngine>

#include <private/qjsvalue_p.h>
#include <private/qqmlmetatype_p.h>
#include <private/qqmldata_p.h>
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
#include <private/qqmlcompiler_p.h>
#endif
#include <private/qqmlcontext_p.h>
#include <private/qqmlscriptstring_p.h>
#include <private/qv8engine_p.h>
#include <private/qv4qobjectwrapper_p.h>

Q_DECLARE_METATYPE(QQmlError)

using namespace GammaRay;

#if defined(QT_DEPRECATED) && QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
static QString metaMethodToString(const QObject *object, const QMetaMethod &method)
{
    return QStringLiteral("%1 bound on %2").arg(method.methodSignature(), Util::displayString(
                                                    object));
}

#endif

static QString callableQjsValueToString(const QJSValue &v)
{
#if defined(QT_DEPRECATED) && QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    // note: QJSValue::engine() is deprecated
    // note: QJSValuePrivate::convertedToValue got introduced in Qt 5.5.0
    QV4::ExecutionEngine *jsEngine = QV8Engine::getV4(v.engine());
    QV4::Scope scope(jsEngine);

    QV4::Scoped<QV4::QObjectMethod> qobjectMethod(scope, QJSValuePrivate::convertedToValue(jsEngine,
                                                                                           v));
    if (!qobjectMethod)
        return QStringLiteral("<callable>");

    QObject *sender = qobjectMethod->object();
    Q_ASSERT(sender);
    QMetaMethod metaMethod = sender->metaObject()->method(qobjectMethod->methodIndex());
    return metaMethodToString(sender, metaMethod);
#else
    Q_UNUSED(v);
    return QStringLiteral("<callable>");
#endif
}

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
    QQmlListProperty<QObject> *prop
        = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(value.data()));
    if (!prop || !prop->count)
        return QString();

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    // note: v.isQMetaObject() == true => v.isCallable() == true, because QV4::QMetaObjectWrapper inherits
    // QV4::FunctionObject and isCallable just checks whether the object is a function object.
    // thus the isQMetaObject check needs to come before the isCallable check
    } else if (v.isQMetaObject()) {
        return QStringLiteral("QMetaObject[className=%1]").arg(v.toQMetaObject()->className());
#endif
    } else if (v.isCallable()) {
        return callableQjsValueToString(v);
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

static QString qqmlScriptStringToString(const QQmlScriptString &v)
{
    // QQmlScriptStringPrivate::get is not guaranteed to be exported, inline
    auto scriptStringPriv
        = reinterpret_cast<const QSharedDataPointer<QQmlScriptStringPrivate> *>(&v)->constData();
    return scriptStringPriv->script;
}

namespace GammaRay {
class QmlObjectDataProvider : public AbstractObjectDataProvider
{
public:
    QString name(const QObject *obj) const override;
    QString typeName(QObject *obj) const override;
    QString shortTypeName(QObject *obj) const override;
    SourceLocation creationLocation(QObject *obj) const override;
    SourceLocation declarationLocation(QObject *obj) const override;
};
}

QString QmlObjectDataProvider::name(const QObject *obj) const
{
    QQmlContext *ctx = QQmlEngine::contextForObject(obj);
    if (!ctx || !ctx->engine())
        return QString(); // nameForObject crashes for contexts that have no engine (yet)

    return ctx->nameForObject(const_cast<QObject *>(obj));
}

QString QmlObjectDataProvider::typeName(QObject *obj) const
{
    Q_ASSERT(obj);

    // C++ QML type
    auto qmlType = QQmlMetaType::qmlType(obj->metaObject());
    // QQC2 has some weird types with only the namespace (that is, ending in '/')
    // we get better results below, so ignore this case here
    if (QmlType::isValid(qmlType) && !QmlType::callable(qmlType)->qmlTypeName().endsWith(QLatin1Char('/')))
        return QmlType::callable(qmlType)->qmlTypeName();

    // QML defined type
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    if (!data || !data->compiledData)
        return QString();

    qmlType = QQmlMetaType::qmlType(data->compiledData->url());
#else
    if (!data || !data->compilationUnit)
        return QString();

    qmlType = QQmlMetaType::qmlType(data->compilationUnit->url());
#endif
    if (QmlType::isValid(qmlType)) {
        // we get the same type for top-level types and inline types, with no known way to tell those apart...
        if (QString::fromLatin1(obj->metaObject()->className()).startsWith(QmlType::callable(qmlType)->qmlTypeName() + QStringLiteral("_QMLTYPE_")))
            return QmlType::callable(qmlType)->qmlTypeName();
    }
#endif
    return QString();
}

QString QmlObjectDataProvider::shortTypeName(QObject *obj) const
{
    auto n = typeName(obj);
    const auto isQmlType = !n.isEmpty();
    if (isQmlType) {
        n = n.section(QLatin1Char('/'), -1, -1); // strip off the namespace
    } else {
        n = obj->metaObject()->className();
    }

    auto idx = n.indexOf(QLatin1String("_QMLTYPE_"));
    if (idx > 0)
        return n.left(idx);

    idx = n.indexOf(QLatin1String("_QML_"));
    if (idx > 0)
        return n.left(idx);

    return isQmlType ? n : QString(); // let somebody else handle shortening of non-QML names
}

SourceLocation QmlObjectDataProvider::creationLocation(QObject *obj) const
{
    SourceLocation loc;

    auto objectData = QQmlData::get(obj);
    if (!objectData) {
        if (auto context = qobject_cast<QQmlContext *>(obj)) {
            loc.setUrl(context->baseUrl());
        }
        return loc;
    }

    auto context = objectData->outerContext;
    if (!context)
        return loc;

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    loc.setUrl(context->url());
#else
    loc.setUrl(context->url);
#endif

    loc.setLine(static_cast<int>(objectData->lineNumber) - 1);
    loc.setColumn(static_cast<int>(objectData->columnNumber) - 1);
    return loc;
}

SourceLocation QmlObjectDataProvider::declarationLocation(QObject *obj) const
{
    Q_ASSERT(obj);

    // C++ QML type
    auto qmlType = QQmlMetaType::qmlType(obj->metaObject());
    if (QmlType::isValid(qmlType))
        return SourceLocation(QmlType::callable(qmlType)->sourceUrl());

    // QML-defined type
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    auto data = QQmlData::get(obj);
#if QT_VERSION < QT_VERSION_CHECK(5, 8, 0)
    if (!data || !data->compiledData)
        return SourceLocation();

    qmlType = QQmlMetaType::qmlType(data->compiledData->url());
#else
    if (!data || !data->compilationUnit)
        return SourceLocation();

    qmlType = QQmlMetaType::qmlType(data->compilationUnit->url());
#endif
    if (QmlType::isValid(qmlType))
        return SourceLocation(QmlType::callable(qmlType)->sourceUrl());
#endif
    return SourceLocation();
}

QmlSupport::QmlSupport(GammaRay::ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QQmlComponent, QObject);
    MO_ADD_PROPERTY_RO(QQmlComponent, QList<QQmlError>, errors);
    MO_ADD_PROPERTY_RO(QQmlComponent, bool, isError);
    MO_ADD_PROPERTY_RO(QQmlComponent, bool, isLoading);
    MO_ADD_PROPERTY_RO(QQmlComponent, bool, isNull);
    MO_ADD_PROPERTY_RO(QQmlComponent, bool, isReady);

    MO_ADD_METAOBJECT1(QQmlContext, QObject);
    MO_ADD_PROPERTY_CR(QQmlContext, QUrl, baseUrl, setBaseUrl);
    MO_ADD_PROPERTY(QQmlContext, QObject *, contextObject, setContextObject);
    MO_ADD_PROPERTY_RO(QQmlContext, QQmlEngine *, engine);
    MO_ADD_PROPERTY_RO(QQmlContext, bool, isValid);
    MO_ADD_PROPERTY_RO(QQmlContext, QQmlContext *, parentContext);

    MO_ADD_METAOBJECT1(QJSEngine, QObject);
    MO_ADD_PROPERTY_RO(QJSEngine, QJSValue, globalObject);

    MO_ADD_METAOBJECT1(QQmlEngine, QJSEngine);
    MO_ADD_PROPERTY_CR(QQmlEngine, QUrl, baseUrl, setBaseUrl);
    MO_ADD_PROPERTY_CR(QQmlEngine, QStringList, importPathList, setImportPathList);
    MO_ADD_PROPERTY(QQmlEngine, bool, outputWarningsToStandardError,
                    setOutputWarningsToStandardError);
    MO_ADD_PROPERTY_CR(QQmlEngine, QStringList, pluginPathList, setPluginPathList);
    MO_ADD_PROPERTY_RO(QQmlEngine, QQmlContext *, rootContext);

    MO_ADD_METAOBJECT0(QQmlType);
    MO_ADD_PROPERTY_RO(QQmlType, QByteArray, typeName);
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 2)
    MO_ADD_PROPERTY_RO(QQmlType, const QString &, qmlTypeName);
    MO_ADD_PROPERTY_RO(QQmlType, const QString &, elementName);
#else
    MO_ADD_PROPERTY_RO(QQmlType, QString, qmlTypeName);
    MO_ADD_PROPERTY_RO(QQmlType, QString, elementName);
#endif
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
// MO_ADD_PROPERTY_RO(QQmlType, const char*, interfaceIId);
    MO_ADD_PROPERTY_RO(QQmlType, int, index);
    MO_ADD_PROPERTY_RO(QQmlType, const QMetaObject *, metaObject);
    MO_ADD_PROPERTY_RO(QQmlType, const QMetaObject *, baseMetaObject);
    MO_ADD_PROPERTY_RO(QQmlType, QUrl, sourceUrl);

    VariantHandler::registerStringConverter<QJSValue>(qjsValueToString);
    VariantHandler::registerStringConverter<QQmlScriptString>(qqmlScriptStringToString);
    VariantHandler::registerStringConverter<QQmlError>(qmlErrorToString);
    VariantHandler::registerGenericStringConverter(qmlListPropertyToString);

    PropertyAdaptorFactory::registerFactory(QmlListPropertyAdaptorFactory::instance());
    PropertyAdaptorFactory::registerFactory(QmlAttachedPropertyAdaptorFactory::instance());
    PropertyAdaptorFactory::registerFactory(QJSValuePropertyAdaptorFactory::instance());
    PropertyAdaptorFactory::registerFactory(QmlContextPropertyAdaptorFactory::instance());

    PropertyController::registerExtension<QmlContextExtension>();
    PropertyController::registerExtension<QmlTypeExtension>();

    static auto dataProvider = new QmlObjectDataProvider;
    ObjectDataProvider::registerProvider(dataProvider);
}
