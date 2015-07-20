/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>
#include <core/util.h>
#include <core/propertyadaptorfactory.h>

#include <QDateTime>
#include <QDebug>
#include <QJSValue>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlError>
#include <QQmlListProperty>

Q_DECLARE_METATYPE(QQmlError)

using namespace GammaRay;

static QString qmlErrorToString(const QQmlError &error)
{
  return QString::fromLatin1("%1:%2:%3: %4")
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
    return "<array>";
  } else if (v.isBool()) {
    return v.toBool() ? "true" : "false";
  } else if (v.isCallable()) {
    return "<callable>";
  } else if (v.isDate()) {
    return v.toDateTime().toString();
  } else if (v.isError()) {
    return "<error>";
  } else if (v.isNull()) {
    return "<null>";
  } else if (v.isNumber()) {
    return QString::number(v.toNumber());
  } else if (v.isObject()) {
    return "<object>";
  } else if (v.isQObject()) {
    return Util::displayString(v.toQObject());
  } else if (v.isRegExp()) {
    return "<regexp>";
  } else if (v.isString()) {
    return v.toString();
  } else if (v.isUndefined()) {
    return "<undefined>";
  } else if (v.isVariant()) {
    return VariantHandler::displayString(v.toVariant());
  }
  return "<unknown QJSValue>";
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

  VariantHandler::registerStringConverter<QJSValue>(qjsValueToString);
  VariantHandler::registerStringConverter<QQmlError>(qmlErrorToString);
  VariantHandler::registerGenericStringConverter(qmlListPropertyToString);

  PropertyAdaptorFactory::registerFactory(QmlListPropertyAdaptorFactory::instance());
  PropertyAdaptorFactory::registerFactory(QmlAttachedPropertyAdaptorFactory::instance());
}

QString QmlSupportFactory::name() const
{
  return tr("QML Support");
}
