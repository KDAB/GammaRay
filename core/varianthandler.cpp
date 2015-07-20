/*
  varianthandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "varianthandler.h"
#include "util.h"
#include "common/metatypedeclarations.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0 , 0)
#include <QApplication>
#else
#include <QGuiApplication>
#endif

#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QMatrix4x4>
#include <QMetaEnum>
#include <QMetaObject>
#include <QObject>
#include <QPainter>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QStringList>
#include <QTextFormat>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

using namespace GammaRay;

namespace GammaRay
{

struct VariantHandlerRepository
{
  QHash<int, VariantHandler::Converter<QString>*> stringConverters;
  QVector<VariantHandler::GenericStringConverter> genericStringConverters;
};

static QString displayMatrix4x4(const QMatrix4x4 &matrix)
{
  QStringList rows;
  rows.reserve(4);
  for (int i = 0; i < 4; ++i) {
    QStringList cols;
    cols.reserve(4);
    for (int j = 0; j < 4; ++j) {
      cols.push_back(QString::number(matrix(i, j)));
    }
    rows.push_back(cols.join(" "));
  }
  return '[' + rows.join(", ") + ']';
}

static QString displayMatrix4x4(const QMatrix4x4 *matrix)
{
  if (matrix) {
    return displayMatrix4x4(*matrix);
  }
  return "<null>";
}

template <int Dim, typename T>
static QString displayVector(const T &vector)
{
  QStringList v;
  for (int i = 0; i < Dim; ++i)
    v.push_back(QString::number(vector[i]));
  return '[' + v.join(", ") + ']';
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
static QString displayShaderType(const QOpenGLShader::ShaderType type)
{
  QStringList types;
#define ST(t) if (type & QOpenGLShader::t) types.push_back(#t);
  ST(Vertex)
  ST(Fragment)
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
  ST(Geometry)
  ST(TessellationControl)
  ST(TessellationEvaluation)
  ST(Compute)
#endif
#undef ST

  if (types.isEmpty())
    return "<none>";
  return types.join(" | ");
}
#endif

}

Q_GLOBAL_STATIC(VariantHandlerRepository, s_variantHandlerRepository)

QString VariantHandler::displayString(const QVariant &value)
{
  switch (value.type()) {
#ifndef QT_NO_CURSOR
  case QVariant::Cursor:
  {
    const QCursor cursor = value.value<QCursor>();
    return Util::enumToString(QVariant::fromValue<int>(cursor.shape()), "Qt::CursorShape");
  }
#endif
  case QVariant::Icon:
  {
    const QIcon icon = value.value<QIcon>();
    if (icon.isNull()) {
      return QObject::tr("<no icon>");
    }
    const auto sizes = icon.availableSizes();
    QStringList l;
    l.reserve(sizes.size());
    foreach (const QSize &size, sizes) {
      l.push_back(displayString(size));
    }
    return l.join(QLatin1String(", "));
  }
  case QVariant::Line:
    return
      QString::fromUtf8("%1, %2 → %3, %4").
        arg(value.toLine().x1()).arg(value.toLine().y1()).
        arg(value.toLine().x2()).arg(value.toLine().y2());

  case QVariant::LineF:
    return
      QString::fromUtf8("%1, %2 → %3, %4").
        arg(value.toLineF().x1()).arg(value.toLineF().y1()).
        arg(value.toLineF().x2()).arg(value.toLineF().y2());

  case QVariant::Locale:
    return value.toLocale().name();

  case QVariant::Point:
    return
      QString::fromLatin1("%1, %2").
        arg(value.toPoint().x()).
        arg(value.toPoint().y());

  case QVariant::PointF:
    return
      QString::fromLatin1("%1, %2").
        arg(value.toPointF().x()).
        arg(value.toPointF().y());

  case QVariant::Rect:
    return
      QString::fromLatin1("%1, %2 %3 x %4").
        arg(value.toRect().x()).
        arg(value.toRect().y()).
        arg(value.toRect().width()).
        arg(value.toRect().height());

  case QVariant::RectF:
    return
      QString::fromLatin1("%1, %2 %3 x %4").
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
      return displayString(region.rects().at(0));
    } else {
      return QString::fromLatin1("<%1 rects>").arg(region.rectCount());
    }
  }

  case QVariant::Palette:
  {
    const QPalette pal = value.value<QPalette>();
    if (pal == qApp->palette()) {
      return QLatin1String("<inherited>");
    }
    return QLatin1String("<custom>");
  }

  case QVariant::Size:
    return
      QString::fromLatin1("%1 x %2").
        arg(value.toSize().width()).
        arg(value.toSize().height());

  case QVariant::SizeF:
    return
      QString::fromLatin1("%1 x %2").
        arg(value.toSizeF().width()).
        arg(value.toSizeF().height());

  case QVariant::StringList:
  {
    const auto l = value.toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (l.isEmpty())
      return QLatin1String("<empty>");
    if (l.size() == 1)
      return l.at(0);
    return QString::fromLatin1("<%1 entries>").arg(l.size());
#else
    return l.join(", ");
#endif
  }

  case QVariant::Transform:
  {
    const QTransform t = value.value<QTransform>();
    return
      QString::fromLatin1("[%1 %2 %3, %4 %5 %6, %7 %8 %9]").
        arg(t.m11()).arg(t.m12()).arg(t.m13()).
        arg(t.m21()).arg(t.m22()).arg(t.m23()).
        arg(t.m31()).arg(t.m32()).arg(t.m33());
  }
  default:
    break;
  }

  // types with dynamic type ids
  if (value.type() == (QVariant::Type)qMetaTypeId<QTextLength>()) {
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

  if (value.userType() == qMetaTypeId<QMargins>()) {
    const QMargins margins = value.value<QMargins>();
    return QObject::tr("left: %1, top: %2, right: %3, bottom: %4")
       .arg(margins.left()).arg(margins.top())
       .arg(margins.right()).arg(margins.bottom());
  }

  if (value.canConvert<QObject*>()) {
    return Util::displayString(value.value<QObject*>());
  }

  if (value.userType() == qMetaTypeId<QMatrix4x4>()) {
    return displayMatrix4x4(value.value<QMatrix4x4>());
  }

  if (value.userType() == qMetaTypeId<const QMatrix4x4*>()) {
    return displayMatrix4x4(value.value<const QMatrix4x4*>());
  }

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  if (value.userType() == qMetaTypeId<QVector2D>())
    return displayVector<2>(value.value<QVector2D>());
  if (value.userType() == qMetaTypeId<QVector3D>())
    return displayVector<3>(value.value<QVector3D>());
  if (value.userType() == qMetaTypeId<QVector4D>())
    return displayVector<4>(value.value<QVector4D>());
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  if (value.userType() == qMetaTypeId<QSet<QByteArray> >()) {
    const QSet<QByteArray> set = value.value<QSet<QByteArray> >();
    QStringList l;
    l.reserve(set.size());
    foreach (const QByteArray &b, set) {
      l.push_back(QString::fromUtf8(b));
    }
    return l.join(", ");
  }

  if (value.userType() == qMetaTypeId<QSurfaceFormat>()) {
    const QSurfaceFormat format = value.value<QSurfaceFormat>();
    QString s;
    switch (format.renderableType()) {
    case QSurfaceFormat::DefaultRenderableType:
      s += "Default";
      break;
    case QSurfaceFormat::OpenGL:
      s += "OpenGL";
      break;
    case QSurfaceFormat::OpenGLES:
      s += "OpenGL ES";
      break;
    case QSurfaceFormat::OpenVG:
      s += "OpenVG";
      break;
    }

    s += " (" + QString::number(format.majorVersion()) +
         '.' + QString::number(format.minorVersion());
    switch (format.profile()) {
    case QSurfaceFormat::CoreProfile:
      s += " core";
      break;
    case QSurfaceFormat::CompatibilityProfile:
      s += " compat";
      break;
    case QSurfaceFormat::NoProfile:
      break;
    }
    s += ')';

    s += " RGBA: " + QString::number(format.redBufferSize()) +
         '/' + QString::number(format.greenBufferSize()) +
         '/' + QString::number(format.blueBufferSize()) +
         '/' + QString::number(format.alphaBufferSize());

    s += " Depth: " + QString::number(format.depthBufferSize());
    s += " Stencil: " + QString::number(format.stencilBufferSize());

    s += " Buffer: ";
    switch (format.swapBehavior()) {
    case QSurfaceFormat::DefaultSwapBehavior:
      s += "default";
      break;
    case QSurfaceFormat::SingleBuffer:
      s += "single";
      break;
    case QSurfaceFormat::DoubleBuffer:
      s += "double";
      break;
    case QSurfaceFormat::TripleBuffer:
      s += "triple";
      break;
    default:
      s += "unknown";
    }

    return s;
  }

  if (value.userType() == qMetaTypeId<QSurface::SurfaceClass>()) {
    const QSurface::SurfaceClass sc = value.value<QSurface::SurfaceClass>();
    switch (sc) {
      case QSurface::Window: return QObject::tr("Window");
#if QT_VERSION > QT_VERSION_CHECK(5, 1, 0)
      case QSurface::Offscreen: return QObject::tr("Offscreen");
#endif
      default: return QObject::tr("Unknown Surface Class");
    }
  }

  if (value.userType() == qMetaTypeId<QSurface::SurfaceType>()) {
    const QSurface::SurfaceType type = value.value<QSurface::SurfaceType>();
    switch (type) {
      case QSurface::RasterSurface: return QObject::tr("Raster");
      case QSurface::OpenGLSurface: return QObject::tr("OpenGL");
      default: return QObject::tr("Unknown Surface Type");
    }
  }

  if (value.userType() == qMetaTypeId<QOpenGLShader::ShaderType>())
    return displayShaderType(value.value<QOpenGLShader::ShaderType>());

#endif // Qt5

  // enums
  const QString enumStr = Util::enumToString(value);
  if (!enumStr.isEmpty()) {
    return enumStr;
  }

  // custom converters
  const QHash<int, Converter<QString>*>::const_iterator it =
    s_variantHandlerRepository()->stringConverters.constFind(value.userType());
  if (it != s_variantHandlerRepository()->stringConverters.constEnd()) {
    return (*it.value())(value);
  }

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  if (value.canConvert<QVariantList>()) {
    QSequentialIterable it = value.value<QSequentialIterable>();
    if (it.size() == 0) {
      return QLatin1String("<empty>");
    } else {
      return QString::fromLatin1("<%1 entries>").arg(it.size());
    }
  }
  if (value.canConvert<QVariantHash>()) {
    auto it = value.value<QAssociativeIterable>();
    if (it.size() == 0) {
      return QLatin1String("<empty>");
    } else {
      return QString::fromLatin1("<%1 entries>").arg(it.size());
    }
  }
#endif

  // generic converters
  QVector<VariantHandler::GenericStringConverter> genStrConverters =
    s_variantHandlerRepository()->genericStringConverters;
  foreach (const GenericStringConverter &converter, genStrConverters) {
    bool ok = false;
    const QString s = converter(value, &ok);
    if (ok) {
      return s;
    }
  }

  return value.toString();
}

QVariant VariantHandler::decoration(const QVariant &value)
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
    break;
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
    break;
  }
#ifndef QT_NO_CURSOR
  case QVariant::Cursor:
  {
    const QCursor c = value.value<QCursor>();
    if (!c.pixmap().isNull()) {
      return c.pixmap().scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation);
    }
    break;
  }
#endif
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
    break;
  }
  case QVariant::Pixmap:
  {
    const QPixmap p = value.value<QPixmap>();
    if(!p.isNull()) {
      return QVariant::fromValue(p.scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation));
    }
    break;
  }
  default: break;
  }

  return QVariant();
}

void VariantHandler::registerStringConverter(int type, Converter<QString> *converter)
{
  s_variantHandlerRepository()->stringConverters.insert(type, converter);
}

void VariantHandler::registerGenericStringConverter(
  VariantHandler::GenericStringConverter converter)
{
  s_variantHandlerRepository()->genericStringConverters.push_back(converter);
}

QVariant VariantHandler::serializableVariant(const QVariant& value)
{
  if (value.userType() == qMetaTypeId<const QMatrix4x4*>()) {
    const QMatrix4x4 *m = value.value<const QMatrix4x4*>();
    if (!m)
      return QVariant();
    return QVariant::fromValue(QMatrix4x4(*m));
  }

  return value;
}
