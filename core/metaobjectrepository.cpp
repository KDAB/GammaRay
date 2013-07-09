/*
  metaobjectrepository.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metaobjectrepository.h"
#include "metaobject.h"

#include "include/metatypedeclarations.h"

#include <QAbstractSocket>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QNetworkProxy>
#include <QObject>
#include <QPalette>
#include <QPen>
#include <QSocketNotifier>
#include <QStyle>
#include <QWidget>
#include <QTcpServer>

#define MO_ADD_BASECLASS(Base) \
  Q_ASSERT(hasMetaObject(QLatin1String(#Base))); \
  mo->addBaseClass(metaObject(QLatin1String(#Base)));

#define MO_ADD_METAOBJECT0(Class) \
  mo = new MetaObjectImpl<Class>; \
  mo->setClassName(QLatin1String(#Class)); \
  addMetaObject(mo);

#define MO_ADD_METAOBJECT1(Class, Base1) \
  mo = new MetaObjectImpl<Class, Base1>; \
  mo->setClassName(QLatin1String(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  addMetaObject(mo);

#define MO_ADD_METAOBJECT2(Class, Base1, Base2) \
  mo = new MetaObjectImpl<Class, Base1, Base2>; \
  mo->setClassName(QLatin1String(#Class)); \
  MO_ADD_BASECLASS(Base1) \
  MO_ADD_BASECLASS(Base2) \
  addMetaObject(mo);

#define MO_ADD_PROPERTY(Class, Type, Getter, Setter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type>( \
    QLatin1String(#Getter), \
    &Class::Getter, \
    static_cast<void (Class::*)(Type)>(&Class::Setter)) \
  );

#define MO_ADD_PROPERTY_CR(Class, Type, Getter, Setter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type, const Type&>( \
    QLatin1String(#Getter), \
    &Class::Getter, \
    static_cast<void (Class::*)(const Type&)>(&Class::Setter)) \
  );

#define MO_ADD_PROPERTY_RO(Class, Type, Getter) \
  mo->addProperty(new MetaPropertyImpl<Class, Type>( \
    QLatin1String(#Getter), \
    &Class::Getter));

using namespace GammaRay;

namespace GammaRay {

class StaticMetaObjectRepository : public MetaObjectRepository
{
  public:
    StaticMetaObjectRepository() : MetaObjectRepository()
    {
    }
};

}

Q_GLOBAL_STATIC(StaticMetaObjectRepository, s_instance)

MetaObjectRepository::MetaObjectRepository()
{
  initBuiltInTypes();
}

MetaObjectRepository::~MetaObjectRepository()
{
  qDeleteAll(m_metaObjects);
}

void MetaObjectRepository::initBuiltInTypes()
{
  initQObjectTypes();
  initGraphicsViewTypes();
  initNetworkTypes();
}

void MetaObjectRepository::initQObjectTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT0(QObject);
  MO_ADD_PROPERTY_RO(QObject, QObject*, parent);
  MO_ADD_PROPERTY_RO(QObject, bool, signalsBlocked); // TODO setter has non-void return type

  MO_ADD_METAOBJECT0(QPaintDevice);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, colorCount);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, heightMM);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiX);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiY);
  MO_ADD_PROPERTY_RO(QPaintDevice, bool, paintingActive);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiX);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiY);
  MO_ADD_PROPERTY_RO(QPaintDevice, int, widthMM);

  MO_ADD_METAOBJECT2(QWidget, QObject, QPaintDevice);
  MO_ADD_PROPERTY_RO(QWidget, QWidget*, focusProxy);

  MO_ADD_METAOBJECT1(QStyle, QObject);
  MO_ADD_PROPERTY_RO(QStyle, const QStyle*, proxy);
  MO_ADD_PROPERTY_RO(QStyle, QPalette, standardPalette);
}

void MetaObjectRepository::initGraphicsViewTypes()
{
  MetaObject *mo = 0; //createMetaObject( "QGraphicsItem" );
  MO_ADD_METAOBJECT0(QGraphicsItem);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptDrops,               setAcceptDrops);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptHoverEvents,         setAcceptHoverEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptTouchEvents,         setAcceptTouchEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::MouseButtons,                 acceptedMouseButtons,      setAcceptedMouseButtons);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           boundingRect);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            boundingRegionGranularity, setBoundingRegionGranularity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem::CacheMode,         cacheMode);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           childrenBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     clipPath);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QCursor,                          cursor,                    setCursor);
  MO_ADD_PROPERTY_RO(QGraphicsItem, qreal,                            effectiveOpacity);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             filtersChildEvents,        setFiltersChildEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, QGraphicsItem::GraphicsItemFlags, flags,                     setFlags);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   focusItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   focusProxy);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsEffect*,                 graphicsEffect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItemGroup*,              group);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             hasCursor);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             hasFocus);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::InputMethodHints,             inputMethodHints,          setInputMethodHints);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isActive);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isClipped);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isEnabled,                 setEnabled);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isObscured);
#endif
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isPanel);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isSelected,                setSelected);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isUnderMouse);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isVisible,                 setVisible);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWidget);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWindow);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            opacity,                   setOpacity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     opaqueArea);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   panel);
  MO_ADD_PROPERTY   (QGraphicsItem, QGraphicsItem::PanelModality,     panelModality,             setPanelModality);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   parentItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsObject*,                 parentObject);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 parentWidget);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          pos,                       setPos);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            rotation,                  setRotation);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            scale,                     setScale);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           sceneBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPointF,                          scenePos);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QTransform,                       sceneTransform);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     shape);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QString,                          toolTip,                   setToolTip);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   topLevelItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 topLevelWidget);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QTransform,                       transform/*,                 setTransform*/); // TODO: support setTransform
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          transformOriginPoint,      setTransformOriginPoint);
  MO_ADD_PROPERTY_RO(QGraphicsItem, int,                              type);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 window);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            x,                         setX);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            y,                         setY);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            zValue,                    setZValue);

  MO_ADD_METAOBJECT1(QAbstractGraphicsShapeItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QBrush, brush, setBrush);
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QPen,   pen,   setPen);

  MO_ADD_METAOBJECT1(QGraphicsEllipseItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsEllipseItem, QRectF, rect,    setRect);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, spanAngle,  setSpanAngle);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, startAngle, setStartAngle);

  MO_ADD_METAOBJECT1(QGraphicsPathItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsPathItem, QPainterPath, path, setPath);

  MO_ADD_METAOBJECT1(QGraphicsPolygonItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY   (QGraphicsPolygonItem, Qt::FillRule, fillRule, setFillRule);
  MO_ADD_PROPERTY_CR(QGraphicsPolygonItem, QPolygonF, polygon, setPolygon);

  MO_ADD_METAOBJECT1(QGraphicsSimpleTextItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsSimpleTextItem, QFont, font, setFont);
  MO_ADD_PROPERTY_CR(QGraphicsSimpleTextItem, QString, text, setText);

  MO_ADD_METAOBJECT1(QGraphicsRectItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsRectItem, QRectF, rect, setRect);

  MO_ADD_METAOBJECT1(QGraphicsLineItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QGraphicsLineItem, QLineF, line, setLine);
  MO_ADD_PROPERTY_CR(QGraphicsLineItem, QPen, pen, setPen);

  MO_ADD_METAOBJECT1(QGraphicsPixmapItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QGraphicsPixmapItem, QPointF, offset, setOffset);
  MO_ADD_PROPERTY_CR(QGraphicsPixmapItem, QPixmap, pixmap, setPixmap);
  MO_ADD_PROPERTY   (QGraphicsPixmapItem, QGraphicsPixmapItem::ShapeMode, shapeMode, setShapeMode);
  MO_ADD_PROPERTY   (QGraphicsPixmapItem, Qt::TransformationMode, transformationMode, setTransformationMode);

  // no extra properties, but we need the inheritance connection for anything above to work
  MO_ADD_METAOBJECT2(QGraphicsObject, QGraphicsItem, QObject);

  MO_ADD_METAOBJECT0(QGraphicsLayoutItem);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, QRectF, contentsRect);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, bool, isLayout);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, bool, ownedByLayout);

  MO_ADD_METAOBJECT2(QGraphicsWidget, QGraphicsObject, QGraphicsLayoutItem);
  MO_ADD_PROPERTY_RO(QGraphicsWidget, QRectF, windowFrameGeometry);
  MO_ADD_PROPERTY_RO(QGraphicsWidget, QRectF, windowFrameRect);

  MO_ADD_METAOBJECT1(QGraphicsProxyWidget, QGraphicsWidget);
  MO_ADD_PROPERTY_RO(QGraphicsProxyWidget, QWidget*, widget);
}

Q_DECLARE_METATYPE(QAbstractSocket::PauseModes)
Q_DECLARE_METATYPE(QAbstractSocket::SocketType)
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(QIODevice::OpenMode)
Q_DECLARE_METATYPE(QSocketNotifier::Type)

void MetaObjectRepository::initNetworkTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QIODevice, QObject);
  MO_ADD_PROPERTY_RO(QIODevice, QIODevice::OpenMode, openMode);
  MO_ADD_PROPERTY   (QIODevice, bool, isTextModeEnabled, setTextModeEnabled);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isOpen);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isReadable);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isWritable);
  MO_ADD_PROPERTY_RO(QIODevice, bool, isSequential);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, pos);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, size);
  MO_ADD_PROPERTY_RO(QIODevice, bool, atEnd);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, bytesAvailable);
  MO_ADD_PROPERTY_RO(QIODevice, qint64, bytesToWrite);
  MO_ADD_PROPERTY_RO(QIODevice, bool, canReadLine);
  MO_ADD_PROPERTY_RO(QIODevice, QString, errorString);

  // FIXME: QIODevice::readAll() would be nice to have

  MO_ADD_METAOBJECT1(QAbstractSocket, QIODevice);
  MO_ADD_PROPERTY   (QAbstractSocket, QAbstractSocket::PauseModes, pauseMode, setPauseMode);
  MO_ADD_PROPERTY_RO(QAbstractSocket, bool, isValid);
  MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, localPort);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, localAddress);
  MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, peerPort);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, peerAddress);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QString, peerName);

  MO_ADD_PROPERTY   (QAbstractSocket, qint64, readBufferSize, setReadBufferSize);
  MO_ADD_PROPERTY_RO(QAbstractSocket, qintptr, socketDescriptor);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketType, socketType);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketState, state);
  MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketError, error);
#ifndef QT_NO_NETWORKPROXY
  MO_ADD_PROPERTY_RO(QAbstractSocket, QNetworkProxy, proxy);
#endif

  // FIXME: QAbstractSocket::setSocketOption() would be nice to have
  // FIXME: QQAbstractSocket::socketOption() would be nice to have

  MO_ADD_METAOBJECT1(QTcpServer, QObject);
  MO_ADD_PROPERTY_RO(QTcpServer, bool, isListening);
  MO_ADD_PROPERTY   (QTcpServer, int, maxPendingConnections, setMaxPendingConnections);
  MO_ADD_PROPERTY_RO(QTcpServer, quint16, serverPort);
  MO_ADD_PROPERTY_RO(QTcpServer, QHostAddress, serverAddress);
  MO_ADD_PROPERTY_RO(QTcpServer, qintptr, socketDescriptor);
  MO_ADD_PROPERTY_RO(QTcpServer, bool, hasPendingConnections);
  MO_ADD_PROPERTY_RO(QTcpServer, QAbstractSocket::SocketError, serverError);
  MO_ADD_PROPERTY_RO(QTcpServer, QString, errorString);
#ifndef QT_NO_NETWORKPROXY
  MO_ADD_PROPERTY_RO(QTcpServer, QNetworkProxy, proxy);
#endif

  MO_ADD_METAOBJECT1(QSocketNotifier, QObject);
  MO_ADD_PROPERTY_RO(QSocketNotifier, qintptr, socket);
  MO_ADD_PROPERTY_RO(QSocketNotifier, QSocketNotifier::Type, type);
  MO_ADD_PROPERTY   (QSocketNotifier, bool, isEnabled, setEnabled);
}

MetaObjectRepository *MetaObjectRepository::instance()
{
  return s_instance();
}

void MetaObjectRepository::addMetaObject(MetaObject *mo)
{
  Q_ASSERT(!mo->className().isEmpty());
  m_metaObjects.insert(mo->className(), mo);
}

MetaObject *MetaObjectRepository::metaObject(const QString &typeName) const
{
  return m_metaObjects.value(typeName);
}

bool MetaObjectRepository::hasMetaObject(const QString &typeName) const
{
  return m_metaObjects.contains(typeName);
}
