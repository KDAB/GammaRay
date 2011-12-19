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
#include "metatypedeclarations.h"

#include <QtCore/qobject.h>
#include <QtCore/QStringList>
#include <qsize.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qmetaobject.h>
#include <QtGui/qtextformat.h>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QWidget>
#include <QDebug>

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

QString Util::addressToUid(const void *p)
{
  return QString::number(reinterpret_cast<qlonglong>(p), 16);
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
  case QVariant::Transform:
  {
    const QTransform t = value.value<QTransform>();
    return QString::fromLatin1("[%1 %2 %3, %4 %5 %6, %7 %8 %9]")
      .arg(t.m11()).arg(t.m12()).arg(t.m13())
      .arg(t.m21()).arg(t.m22()).arg(t.m23())
      .arg(t.m31()).arg(t.m32()).arg(t.m33());
  }
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

  if (value.type() == qMetaTypeId<QWidget*>())
    return displayString(value.value<QWidget*>());

  if (value.userType() == qMetaTypeId<QGraphicsEffect*>())
    return addressToString(value.value<QGraphicsEffect*>());
  if (value.userType() == qMetaTypeId<QGraphicsItem*>())
    return addressToString(value.value<QGraphicsItem*>());
  if (value.userType() == qMetaTypeId<QGraphicsItemGroup*>())
    return addressToString(value.value<QGraphicsItemGroup*>());
  if (value.userType() == qMetaTypeId<QGraphicsObject*>())
    return displayString(value.value<QGraphicsObject*>());
  if (value.userType() == qMetaTypeId<QGraphicsWidget*>())
    return displayString(value.value<QGraphicsWidget*>());

  return value.toString();
}

QVariant Util::decorationForVariant(const QVariant& value)
{
  // TODO: pen, brush, color, cursor
  switch (value.type()) {
    case QVariant::Icon:
      return value;
    case QVariant::Pixmap:
    {
      const QPixmap p = value.value<QPixmap>();
      return QVariant::fromValue( p.scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
    default: break;
  }

  return QVariant();
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
