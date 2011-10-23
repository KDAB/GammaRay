/*
  util.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "util.h"

#include <QtCore/qobject.h>
#include <QtCore/QStringList>
#include <qsize.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qmetaobject.h>
#include <QtGui/qtextformat.h>

using namespace GammaRay;

QString Util::displayString(const QObject *object)
{
  if (!object) {
    return "QObject(0x0)";
  }
  if (object->objectName().isEmpty()) {
    return QString::fromLatin1("%1 (%2)").
      arg(addressToString(object)).
      arg(object->metaObject()->className());
  }
  return object->objectName();
}

static QString sizePolicyToString(QSizePolicy::Policy policy)
{
  const int index = QSizePolicy::staticMetaObject.indexOfEnumerator("Policy");
  const QMetaEnum metaEnum = QSizePolicy::staticMetaObject.enumerator(index);
  return QString::fromLatin1(metaEnum.valueToKey(policy));
}

QString GammaRay::Util::variantToString(const QVariant &value)
{
  switch (value.type()) {
  case QVariant::Point:
    return QString::fromLatin1("%1x%2").
      arg(value.toPoint().x()).
      arg(value.toPoint().y());

  case QVariant::PointF:
    return QString::fromLatin1("%1x%2").
      arg(value.toPointF().x()).
      arg(value.toPointF().y());

  case QVariant::Rect:
    return QString::fromLatin1("%1x%2 %3x%4").
      arg(value.toRect().x()).
      arg(value.toRect().y()).
      arg(value.toRect().width()).
      arg(value.toRect().height());

  case QVariant::RectF:
    return QString::fromLatin1("%1x%2 %3x%4").
      arg(value.toRectF().x()).
      arg(value.toRectF().y()).
      arg(value.toRectF().width()).
      arg(value.toRectF().height());

  case QVariant::Size:
    return QString::fromLatin1("%1x%2").
      arg(value.toSize().width()).
      arg(value.toSize().height());

  case QVariant::SizeF:
    return QString::fromLatin1("%1x%2").
      arg(value.toSizeF().width()).
      arg(value.toSizeF().height());

  case QVariant::SizePolicy:
    return QString::fromLatin1("%1 x %2").
      arg(sizePolicyToString(value.value<QSizePolicy>().horizontalPolicy())).
      arg(sizePolicyToString(value.value<QSizePolicy>().verticalPolicy()));
  case QVariant::StringList:
    return value.toStringList().join(", ");
  default:
    break;
  }

  // types with dynamic type ids
  if (value.type() == qMetaTypeId<QTextLength>()) {
    const QTextLength l = value.value<QTextLength>();
    QString typeStr;
    switch (l.type()) {
    case QTextLength::VariableLength:
      typeStr = QObject::tr("variable");
      break;
    case QTextLength::FixedLength:
      typeStr = QObject::tr("fixed");
      break;
    case QTextLength::PercentageLength:
      typeStr = QObject::tr("percentage");
      break;
    }
    return QString::fromLatin1("%1 (%2)").arg(l.rawValue()).arg(typeStr);
  }

  return value.toString();
}

QString Util::addressToString(const void *p)
{
  return (QLatin1String("0x") + QString::number(reinterpret_cast<qlonglong>(p), 16));
}

bool Util::descendantOf(QObject *ascendant, QObject *obj)
{
  QObject *parent = obj->parent();
  if (!parent) {
    return false;
  }
  if (parent == ascendant) {
    return true;
  }
  return descendantOf(ascendant, parent);
}
