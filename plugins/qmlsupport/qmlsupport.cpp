/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qmlsupport.h"
#include "qmllistpropertyadaptor.h"
#include "qmlattachedpropertyadaptor.h"
#include "qjsvaluepropertyadaptor.h"
#include "qmlcontextpropertyadaptor.h"
#include "qmlcontextextension.h"
#include "qmltypeextension.h"
#include "qmltypeutil.h"
#include "qmlbindingprovider.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>
#include <core/util.h>
#include <core/propertyadaptorfactory.h>
#include <core/propertycontroller.h>
#include <core/objectdataprovider.h>
#include <core/bindingaggregator.h>

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
#include <private/qqmlcontext_p.h>
#include <private/qqmlscriptstring_p.h>
#include <private/qv4qobjectwrapper_p.h>
#include <private/qqmlcontextdata_p.h>

Q_DECLARE_METATYPE(QQmlError)

using namespace GammaRay;

#if defined(QT_DEPRECATED)
static QString metaMethodToString(const QObject *object, const QMetaMethod &method)
{
    return QStringLiteral("%1 bound on %2").arg(method.methodSignature(), Util::displayString(object));
}

#endif

static QString callableQjsValueToString(const QJSValue &v)
{
    QV4::ExecutionEngine *jsEngine = QJSValuePrivate::engine(&v);

    QV4::Scope scope(jsEngine);

    QV4::Scoped<QV4::QObjectMethod> qobjectMethod(scope, QJSValuePrivate::convertToReturnedValue(jsEngine, v));

    if (!qobjectMethod)
        return QStringLiteral("<callable>");

    QObject *sender = qobjectMethod->object();
    Q_ASSERT(sender);
    QMetaMethod metaMethod = sender->metaObject()->method(qobjectMethod->methodIndex());
    return metaMethodToString(sender, metaMethod);
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
    QQmlListProperty<QObject> *prop = reinterpret_cast<QQmlListProperty<QObject> *>(const_cast<void *>(value.data()));
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
        // note: v.isQMetaObject() == true => v.isCallable() == true, because QV4::QMetaObjectWrapper inherits
        // QV4::FunctionObject and isCallable just checks whether the object is a function object.
        // thus the isQMetaObject check needs to come before the isCallable check
    } else if (v.isQMetaObject()) {
        return QStringLiteral("QMetaObject[className=%1]").arg(v.toQMetaObject()->className());
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
    } else if (QVariant var = v.toVariant(); var.isValid()) {
        return VariantHandler::displayString(var);
    }
    return QStringLiteral("<unknown QJSValue>");
}

static QString qqmlScriptStringToString(const QQmlScriptString &v)
{
    // QQmlScriptStringPrivate::get is not guaranteed to be exported, inline
    auto scriptStringPriv = reinterpret_cast<const QSharedDataPointer<QQmlScriptStringPrivate> *>(&v)->constData();
    return scriptStringPriv->script;
}

namespace GammaRay {
class QmlObjectDataProvider : public AbstractObjectDataProvider
{
public:
    QString name(const QObject *obj) const override;
    QString typeName(const QObject *obj) const override;
    QString shortTypeName(const QObject *obj) const override;
    SourceLocation creationLocation(const QObject *obj) const override;
    SourceLocation declarationLocation(const QObject *obj) const override;
};
}

QString QmlObjectDataProvider::name(const QObject *obj) const
{
    QQmlContext *ctx = QQmlEngine::contextForObject(obj);
    if (!ctx || !ctx->engine())
        return QString(); // nameForObject crashes for contexts that have no engine (yet)

    return ctx->nameForObject(const_cast<QObject *>(obj));
}

QString QmlObjectDataProvider::typeName(const QObject *obj) const
{
    Q_ASSERT(obj);

    // C++ QML type
    auto qmlType = QQmlMetaType::qmlType(obj->metaObject());
    // QQC2 has some weird types with only the namespace (that is, ending in '/')
    // we get better results below, so ignore this case here
    if (QmlType::isValid(qmlType) && !QmlType::callable(qmlType)->qmlTypeName().endsWith(QLatin1Char('/')))
        return QmlType::callable(qmlType)->qmlTypeName();

    // QML defined type
    auto data = QQmlData::get(obj);
    if (!data || !data->compilationUnit)
        return QString();

    qmlType = QQmlMetaType::qmlType(data->compilationUnit->url());
    if (QmlType::isValid(qmlType)) {
        // we get the same type for top-level types and inline types, with no known way to tell those apart...
        if (QString::fromLatin1(obj->metaObject()->className()).startsWith(QmlType::callable(qmlType)->qmlTypeName() + QStringLiteral("_QMLTYPE_")))
            return QmlType::callable(qmlType)->qmlTypeName();
    }
    return QString();
}

QString QmlObjectDataProvider::shortTypeName(const QObject *obj) const
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

    return isQmlType ? std::move(n) : QString(); // let somebody else handle shortening of non-QML names
}

SourceLocation QmlObjectDataProvider::creationLocation(const QObject *obj) const
{
    SourceLocation loc;

    auto objectData = QQmlData::get(obj);
    if (!objectData) {
        if (auto context = qobject_cast<const QQmlContext *>(obj)) {
            loc.setUrl(context->baseUrl());
        }
        return loc;
    }

    auto context = objectData->outerContext;
    if (!context)
        return loc;

    loc.setUrl(context->url());
    loc.setOneBasedLine(static_cast<int>(objectData->lineNumber));
    loc.setOneBasedColumn(static_cast<int>(objectData->columnNumber));
    return loc;
}

SourceLocation QmlObjectDataProvider::declarationLocation(const QObject *obj) const
{
    Q_ASSERT(obj);

    // C++ QML type
    auto qmlType = QQmlMetaType::qmlType(obj->metaObject());
    if (QmlType::isValid(qmlType))
        return SourceLocation(QmlType::callable(qmlType)->sourceUrl());

    // QML-defined type
    auto data = QQmlData::get(obj);
    if (!data || !data->compilationUnit)
        return SourceLocation();

    qmlType = QQmlMetaType::qmlType(data->compilationUnit->url());
    if (QmlType::isValid(qmlType))
        return SourceLocation(QmlType::callable(qmlType)->sourceUrl());
    return SourceLocation();
}

QmlSupport::QmlSupport(Probe *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QQmlComponent, QObject);
    MO_ADD_PROPERTY_RO(QQmlComponent, errors);
    MO_ADD_PROPERTY_RO(QQmlComponent, isError);
    MO_ADD_PROPERTY_RO(QQmlComponent, isLoading);
    MO_ADD_PROPERTY_RO(QQmlComponent, isNull);
    MO_ADD_PROPERTY_RO(QQmlComponent, isReady);

    MO_ADD_METAOBJECT1(QQmlContext, QObject);
    MO_ADD_PROPERTY(QQmlContext, baseUrl, setBaseUrl);
    MO_ADD_PROPERTY(QQmlContext, contextObject, setContextObject);
    MO_ADD_PROPERTY_RO(QQmlContext, engine);
    MO_ADD_PROPERTY_RO(QQmlContext, isValid);
    MO_ADD_PROPERTY_RO(QQmlContext, parentContext);

    MO_ADD_METAOBJECT1(QJSEngine, QObject);
    MO_ADD_PROPERTY_RO(QJSEngine, globalObject);

    MO_ADD_METAOBJECT1(QQmlEngine, QJSEngine);
    MO_ADD_PROPERTY(QQmlEngine, baseUrl, setBaseUrl);
    MO_ADD_PROPERTY(QQmlEngine, importPathList, setImportPathList);
    MO_ADD_PROPERTY(QQmlEngine, outputWarningsToStandardError, setOutputWarningsToStandardError);
    MO_ADD_PROPERTY(QQmlEngine, pluginPathList, setPluginPathList);
    MO_ADD_PROPERTY_RO(QQmlEngine, rootContext);

    MO_ADD_METAOBJECT0(QQmlType);
    MO_ADD_PROPERTY_RO(QQmlType, typeName);
    MO_ADD_PROPERTY_RO(QQmlType, qmlTypeName);
    MO_ADD_PROPERTY_RO(QQmlType, elementName);
    MO_ADD_PROPERTY_RO(QQmlType, version);
    MO_ADD_PROPERTY_RO(QQmlType, isCreatable);
    MO_ADD_PROPERTY_RO(QQmlType, isExtendedType);
    MO_ADD_PROPERTY_RO(QQmlType, isSingleton);
    MO_ADD_PROPERTY_RO(QQmlType, isInterface);
    MO_ADD_PROPERTY_RO(QQmlType, isComposite);
    MO_ADD_PROPERTY_RO(QQmlType, isCompositeSingleton);
    MO_ADD_PROPERTY_RO(QQmlType, noCreationReason);
    MO_ADD_PROPERTY_RO(QQmlType, typeId);
    MO_ADD_PROPERTY_RO(QQmlType, qListTypeId);
    MO_ADD_PROPERTY_RO(QQmlType, metaObjectRevision);
    MO_ADD_PROPERTY_RO(QQmlType, containsRevisionedAttributes);
    // MO_ADD_PROPERTY_RO(QQmlType, interfaceIId);
    MO_ADD_PROPERTY_RO(QQmlType, index);
    MO_ADD_PROPERTY_RO(QQmlType, metaObject);
    MO_ADD_PROPERTY_RO(QQmlType, baseMetaObject);
    MO_ADD_PROPERTY_RO(QQmlType, sourceUrl);

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

    BindingAggregator::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QmlBindingProvider));

    static auto dataProvider = new QmlObjectDataProvider;
    ObjectDataProvider::registerProvider(dataProvider);
}
