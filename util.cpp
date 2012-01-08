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
#include <qtextformat.h>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QWidget>
#include <QDebug>
#include <QPainter>
#include <QDir>
#include <QIcon>

using namespace GammaRay;

namespace GammaRay {
class ProtectedExposer : public QObject
{
public:
  using QObject::staticQtMetaObject;
};
}

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
  case QVariant::Line:
    return QString::fromUtf8("%1 x %2 → %3 x %4")
      .arg(value.toLine().x1()).arg(value.toLine().y1())
      .arg(value.toLine().x2()).arg(value.toLine().y2());

  case QVariant::LineF:
    return QString::fromUtf8("%1 x %2 → %3 x %4")
      .arg(value.toLineF().x1()).arg(value.toLineF().y1())
      .arg(value.toLineF().x2()).arg(value.toLineF().y2());

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

  case QVariant::Region:
  {
    const QRegion region = value.value<QRegion>();
    if (region.isEmpty()) {
      return QLatin1String("<empty>");
    }
    if (region.rectCount() == 1) {
      return variantToString(region.rects().first());
    } else {
      return QString::fromLatin1("<%1 rects>").arg(region.rectCount());
    }
  }

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
    return QString::fromLatin1("[%1 %2 %3, %4 %5 %6, %7 %8 %9]").
      arg(t.m11()).arg(t.m12()).arg(t.m13()).
      arg(t.m21()).arg(t.m22()).arg(t.m23()).
      arg(t.m31()).arg(t.m32()).arg(t.m33());
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

  if (value.userType() == qMetaTypeId<QPainterPath>()) {
    const QPainterPath path = value.value<QPainterPath>();
    if (path.isEmpty()) {
      return QObject::tr("<empty>");
    }
    return QObject::tr("<%1 elements>").arg(path.elementCount());
  }

  if (value.type() == qMetaTypeId<QWidget*>()) {
    return displayString(value.value<QWidget*>());
  }

  if (value.userType() == qMetaTypeId<QGraphicsEffect*>()) {
    return addressToString(value.value<QGraphicsEffect*>());
  }
  if (value.userType() == qMetaTypeId<QGraphicsItem*>()) {
    return addressToString(value.value<QGraphicsItem*>());
  }
  if (value.userType() == qMetaTypeId<QGraphicsItemGroup*>()) {
    return addressToString(value.value<QGraphicsItemGroup*>());
  }
  if (value.userType() == qMetaTypeId<QGraphicsObject*>()) {
    return displayString(value.value<QGraphicsObject*>());
  }
  if (value.userType() == qMetaTypeId<QGraphicsWidget*>()) {
    return displayString(value.value<QGraphicsWidget*>());
  }
  if (value.userType() == qMetaTypeId<const QStyle*>()) {
    return displayString(value.value<const QStyle*>());
  }

  // enums
  const QString enumStr = enumToString(value);
  if (!enumStr.isEmpty()) {
    return enumStr;
  }

  return value.toString();
}

QVariant Util::decorationForVariant(const QVariant &value)
{
  switch (value.type()) {
  case QVariant::Brush:
  {
    const QBrush b = value.value<QBrush>();
    if (b.style() != Qt::NoBrush) {
      QPixmap p(16, 16);
      p.fill(QColor(0, 0, 0, 0));
      QPainter painter(&p);
      painter.setBrush(b);
      painter.drawRect(0, 0, p.width() - 1, p.height() - 1);
      return p;
    }
  }
  case QVariant::Color:
  {
    const QColor c = value.value<QColor>();
    if (c.isValid()) {
      QPixmap p(16, 16);
      QPainter painter(&p);
      painter.setBrush(QBrush(c));
      painter.drawRect(0, 0, p.width() - 1, p.height() - 1);
      return p;
    }
  }
  case QVariant::Cursor:
  {
    const QCursor c = value.value<QCursor>();
    if (!c.pixmap().isNull()) {
      return c.pixmap().scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation);
    }
  }
  case QVariant::Icon:
  {
    return value;
  }
  case QVariant::Pen:
  {
    const QPen pen = value.value<QPen>();
    if (pen.style() != Qt::NoPen) {
      QPixmap p(16, 16);
      p.fill(QColor(0, 0, 0, 0));
      QPainter painter(&p);
      painter.setPen(pen);
      painter.translate(0, 8 - pen.width() / 2);
      painter.drawLine(0, 0, p.width(), 0);
      return p;
    }
  }
  case QVariant::Pixmap:
  {
    const QPixmap p = value.value<QPixmap>();
    return QVariant::fromValue(p.scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation));
  }
  default: break;
  }

  return QVariant();
}

QString Util::addressToString(const void *p)
{
  return (QLatin1String("0x") + QString::number(reinterpret_cast<qlonglong>(p), 16));
}

QString Util::enumToString(const QVariant &value, const char *typeName, QObject *object)
{
  QByteArray enumTypeName(typeName);
  if (enumTypeName.isEmpty()) {
    enumTypeName = value.typeName();
  }

  // strip of class name and namespace
  const int pos = enumTypeName.lastIndexOf("::");
  if (pos >= 0) {
    enumTypeName = enumTypeName.mid(pos + 2);
  }

  const QMetaObject *mo = &ProtectedExposer::staticQtMetaObject;
  int enumIndex = mo->indexOfEnumerator(enumTypeName);
  if (enumIndex < 0 && object) {
    mo = object->metaObject();
    enumIndex = mo->indexOfEnumerator(enumTypeName);
  }
  if (enumIndex < 0) {
    return QString();
  }

  const QMetaEnum me = mo->enumerator(enumIndex);
  if (!me.isValid()) {
    return QString();
  }
  return me.valueToKeys(value.toInt());
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

namespace GammaRay {
static QString stringifyProperty(QObject *obj, const QString &propName)
{
  const QVariant value = obj->property(propName.toLatin1());
  const QMetaProperty mp =
    obj->metaObject()->property(
      obj->metaObject()->indexOfProperty(propName.toLatin1()));
  if (mp.isValid()) {
    const QString enumStr = Util::enumToString(value, mp.typeName(), obj);
    if (!enumStr.isEmpty()) {
      return enumStr;
    }
  }
  return Util::variantToString(value);
}

static QVariant iconForObject(const QMetaObject *mo, QObject *obj)
{
  const QString basePath = QString::fromLatin1(":/gammaray/classes/%1/").arg(mo->className());
  const QDir dir(basePath);
  if (dir.exists()) {
    // see if we find one with exactly matching properties
    foreach (const QString &entry,
             dir.entryList(QStringList() << QLatin1String("*.png"), QDir::Files)) {
      if (entry == QLatin1String("default.png")) {
        continue;
      }
      QString propString(entry);
      propString.chop(4);
      const QStringList props = propString.split(QLatin1String(";"));
      if (props.isEmpty()) {
        continue;
      }
      bool allMatch = true;
      foreach (const QString &prop, props) {
        const QStringList keyValue = prop.split(QLatin1Char('='));
        if (keyValue.size() != 2) {
          continue;
        }
        if (stringifyProperty(obj, keyValue.first()) != keyValue.last()) {
          allMatch = false;
          break;
        }
      }
      if (allMatch) {
        return QIcon(basePath + entry);
      }
    }
    return QIcon(basePath + QLatin1String("default.png"));
  } else if (mo->superClass()) {
    return iconForObject(mo->superClass(), obj);
  }
  return QVariant();
}
}

QVariant Util::iconForObject(QObject *obj)
{
  if (obj) {
    return GammaRay::iconForObject(obj->metaObject(), obj);
  }
  return QVariant();
}

