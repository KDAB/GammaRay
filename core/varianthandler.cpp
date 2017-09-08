/*
  varianthandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "enumutil.h"
#include "enumrepositoryserver.h"

#include <common/metatypedeclarations.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
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
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

using namespace GammaRay;

namespace GammaRay {
class VariantHandlerRepository
{
public:
    VariantHandlerRepository() {};
    ~VariantHandlerRepository();
    void clear();

    QHash<int, VariantHandler::Converter<QString> *> stringConverters;
    QVector<VariantHandler::GenericStringConverter> genericStringConverters;

private:
    Q_DISABLE_COPY(VariantHandlerRepository)
};

VariantHandlerRepository::~VariantHandlerRepository()
{
    qDeleteAll(stringConverters);
}

void VariantHandlerRepository::clear()
{
    qDeleteAll(stringConverters);
    stringConverters.clear();
    genericStringConverters.clear();
}

static QString displayMatrix4x4(const QMatrix4x4 &matrix)
{
    QStringList rows;
    rows.reserve(4);
    for (int i = 0; i < 4; ++i) {
        QStringList cols;
        cols.reserve(4);
        for (int j = 0; j < 4; ++j)
            cols.push_back(QString::number(matrix(i, j)));
        rows.push_back(cols.join(QStringLiteral(" ")));
    }
    return '[' + rows.join(QStringLiteral(", ")) + ']';
}

static QString displayMatrix4x4(const QMatrix4x4 *matrix)
{
    if (matrix)
        return displayMatrix4x4(*matrix);
    return QStringLiteral("<null>");
}

template<int Dim, typename T>
static QString displayVector(const T &vector)
{
    QStringList v;
    for (int i = 0; i < Dim; ++i)
        v.push_back(QString::number(vector[i]));
    return '[' + v.join(QStringLiteral(", ")) + ']';
}
}

Q_GLOBAL_STATIC(VariantHandlerRepository, s_variantHandlerRepository)

QString VariantHandler::displayString(const QVariant &value)
{
    switch (value.type()) {
#ifndef QT_NO_CURSOR
    case QVariant::Cursor:
    {
        const QCursor cursor = value.value<QCursor>();
        return EnumUtil::enumToString(QVariant::fromValue<int>(cursor.shape()), "Qt::CursorShape");
    }
#endif
    case QVariant::Icon:
    {
        const QIcon icon = value.value<QIcon>();
        if (icon.isNull())
            return qApp->translate("GammaRay::VariantHandler", "<no icon>");
        const auto sizes = icon.availableSizes();
        QStringList l;
        l.reserve(sizes.size());
        foreach (QSize size, sizes)
            l.push_back(displayString(size));
        return l.join(QStringLiteral(", "));
    }
    case QVariant::Line:
    {
        const auto line = value.toLine();
        return
            QString().sprintf("%i, %i → %i, %i",
                              line.x1(), line.y1(),
                              line.x2(), line.y2());
    }

    case QVariant::LineF:
    {
        const auto line = value.toLineF();
        return
            QString().sprintf("%f, %f → %f, %f",
                              line.x1(), line.y1(),
                              line.x2(), line.y2());
    }

    case QVariant::Locale:
        return value.toLocale().name();

    case QVariant::Pen:
    {
        const auto pen = value.value<QPen>();
        switch (pen.style()) {
        case Qt::NoPen:
            return QStringLiteral("NoPen");
        case Qt::SolidLine:
            return QStringLiteral("SolidLine");
        case Qt::DashLine:
            return QStringLiteral("DashLine");
        case Qt::DotLine:
            return QStringLiteral("DotLine");
        case Qt::DashDotLine:
            return QStringLiteral("DashDotLine");
        case Qt::DashDotDotLine:
            return QStringLiteral("DashDotDotLine");
        case Qt::CustomDashLine:
            return QStringLiteral("CustomDashLine");
#if !defined(Q_MOC_RUN)
        case Qt::MPenStyle:
            return QString();
#endif
        }
        break;
    }

    case QVariant::Point:
    {
        const auto point = value.toPoint();
        return
            QString().sprintf("%i, %i",
                              point.x(), point.y());
    }

    case QVariant::PointF:
    {
        const auto point = value.toPointF();
        return
            QString().sprintf("%f, %f",
                              point.x(), point.y());
    }

    case QVariant::Rect:
    {
        const auto rect = value.toRect();
        return
            QString().sprintf("%i, %i %i x %i",
                              rect.x(), rect.y(),
                              rect.width(), rect.height());
    }

    case QVariant::RectF:
    {
        const auto rect = value.toRectF();
        return
            QString().sprintf("%f, %f %f x %f",
                              rect.x(), rect.y(),
                              rect.width(), rect.height());
    }

    case QVariant::Region:
    {
        const QRegion region = value.value<QRegion>();
        if (region.isEmpty())
            return QStringLiteral("<empty>");
        if (region.rectCount() == 1)
            return displayString(region.rects().at(0));
        else
            return QStringLiteral("<%1 rects>").arg(region.rectCount());
    }

    case QVariant::Palette:
    {
        const QPalette pal = value.value<QPalette>();
        if (pal == qApp->palette())
            return QStringLiteral("<inherited>");
        return QStringLiteral("<custom>");
    }

    case QVariant::Size:
    {
        const auto size = value.toSize();
        return
            QString().sprintf("%i x %i",
                              size.width(), size.height());
    }

    case QVariant::SizeF:
    {
        const auto size = value.toSizeF();
        return
            QString().sprintf("%f x %f",
                              size.width(), size.height());
    }

    case QVariant::StringList:
    {
        const auto l = value.toStringList();
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        if (l.isEmpty())
            return QStringLiteral("<empty>");
        if (l.size() == 1)
            return l.at(0);
        return QStringLiteral("<%1 entries>").arg(l.size());
#else
        return l.join(", ");
#endif
    }

    case QVariant::Transform:
    {
        const QTransform t = value.value<QTransform>();
        return QString().sprintf("[%f %f %f, %f %f %f, %f %f %f]",
                                 t.m11(), t.m12(), t.m13(),
                                 t.m21(), t.m22(), t.m23(),
                                 t.m31(), t.m32(), t.m33());
        }
    default:
        break;
    }

    // types with dynamic type ids
    if (value.userType() == qMetaTypeId<QMargins>()) {
        const QMargins margins = value.value<QMargins>();
        return qApp->translate("GammaRay::VariantHandler", "left: %1, top: %2, right: %3, bottom: %4").arg(
                    QString::number(margins.left()), QString::number(margins.top()),
                    QString::number(margins.right()), QString::number(margins.bottom()));
    }

    if (value.canConvert<QObject *>())
        return Util::displayString(value.value<QObject *>());

    if (value.userType() == qMetaTypeId<const QMetaObject *>()) {
        const auto mo = value.value<const QMetaObject *>();
        if (!mo)
            return QStringLiteral("0x0");
        return mo->className();
    }

    if (value.userType() == qMetaTypeId<QMatrix4x4>())
        return displayMatrix4x4(value.value<QMatrix4x4>());

    if (value.userType() == qMetaTypeId<const QMatrix4x4 *>())
        return displayMatrix4x4(value.value<const QMatrix4x4 *>());

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (value.userType() == qMetaTypeId<QVector2D>())
        return displayVector<2>(value.value<QVector2D>());
    if (value.userType() == qMetaTypeId<QVector3D>())
        return displayVector<3>(value.value<QVector3D>());
    if (value.userType() == qMetaTypeId<QVector4D>())
        return displayVector<4>(value.value<QVector4D>());

    if (value.userType() == qMetaTypeId<QTimeZone>())
        return value.value<QTimeZone>().id();
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (value.userType() == qMetaTypeId<QSet<QByteArray> >()) {
        const QSet<QByteArray> set = value.value<QSet<QByteArray> >();
        QStringList l;
        l.reserve(set.size());
        foreach (const QByteArray &b, set)
            l.push_back(QString::fromUtf8(b));
        return l.join(QStringLiteral(", "));
    }
#endif // Qt5

    // enums
    const QString enumStr = EnumUtil::enumToString(value);
    if (!enumStr.isEmpty())
        return enumStr;

    // custom converters
    auto it = s_variantHandlerRepository()->stringConverters.constFind(value.userType());
    if (it != s_variantHandlerRepository()->stringConverters.constEnd())
        return (*it.value())(value);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (value.canConvert<QVariantList>()) {
        QSequentialIterable it = value.value<QSequentialIterable>();
        if (it.size() == 0)
            return QStringLiteral("<empty>");
        else
            return QStringLiteral("<%1 entries>").arg(it.size());
    }
    if (value.canConvert<QVariantHash>()) {
        auto it = value.value<QAssociativeIterable>();
        if (it.size() == 0)
            return QStringLiteral("<empty>");
        else
            return QStringLiteral("<%1 entries>").arg(it.size());
    }
#endif

    // generic converters
    QVector<VariantHandler::GenericStringConverter> genStrConverters
        = s_variantHandlerRepository()->genericStringConverters;
    foreach (auto converter, genStrConverters) {
        bool ok = false;
        const QString s = converter(value, &ok);
        if (ok)
            return s;
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
            Util::drawTransparencyPattern(&painter, p.rect(), 4);
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
        if (!c.pixmap().isNull())
            return c.pixmap().scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation);
        break;
    }
#endif
    case QVariant::Icon:
        return value;
    case QVariant::Pen:
    {
        const QPen pen = value.value<QPen>();
        if (pen.style() != Qt::NoPen) {
            QPixmap p(16, 16);
            QPainter painter(&p);
            Util::drawTransparencyPattern(&painter, p.rect(), 4);
            painter.save();
            painter.setPen(pen);
            painter.translate(0, 8 - pen.width() / 2);
            painter.drawLine(0, 0, p.width(), 0);
            painter.restore();
            painter.drawRect(0, 0, p.width() - 1, p.height() - 1);
            return p;
        }
        break;
    }
    case QVariant::Pixmap:
    {
        const QPixmap p = value.value<QPixmap>();
        if (!p.isNull())
            return QVariant::fromValue(p.scaled(16, 16, Qt::KeepAspectRatio,
                                                Qt::FastTransformation));
        break;
    }
    default:
        break;
    }

    return QVariant();
}

void VariantHandler::registerStringConverter(int type, Converter<QString> *converter)
{
    Q_ASSERT(!s_variantHandlerRepository()->stringConverters.contains(type));
    s_variantHandlerRepository()->stringConverters.insert(type, converter);
}

void VariantHandler::registerGenericStringConverter(
    VariantHandler::GenericStringConverter converter)
{
    s_variantHandlerRepository()->genericStringConverters.push_back(converter);
}

QVariant VariantHandler::serializableVariant(const QVariant &value)
{
    if (value.userType() == qMetaTypeId<const QMatrix4x4 *>()) {
        const QMatrix4x4 *m = value.value<const QMatrix4x4 *>();
        if (!m)
            return QVariant();
        return QVariant::fromValue(QMatrix4x4(*m));
    }
    if (EnumRepositoryServer::isEnum(value.userType()))
        return QVariant::fromValue(EnumRepositoryServer::valueFromVariant(value));

    return value;
}

void VariantHandler::clear()
{
    s_variantHandlerRepository()->clear();
}
