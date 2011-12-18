#include "metaobjectrepository.h"
#include "metaobject.h"

#include <qglobal.h>
#include <qgraphicsitem.h>
#include <qpen.h>
#include <qobject.h>
#include <qwidget.h>

#define MO_ADD_BASECLASS(Base) \
  Q_ASSERT( hasMetaObject( QLatin1String( #Base ) ) ); \
  mo->addBaseClass( metaObject( QLatin1String( #Base ) ) );

#define MO_ADD_METAOBJECT0(Class) \
  mo = new MetaObjectImpl<Class>; \
  mo->setClassName( QLatin1String( #Class ) ); \
  addMetaObject(mo);

#define MO_ADD_METAOBJECT1(Class, Base1) \
  mo = new MetaObjectImpl<Class, Base1>; \
  mo->setClassName( QLatin1String( #Class ) ); \
  MO_ADD_BASECLASS( Base1 ) \
  addMetaObject(mo);

#define MO_ADD_METAOBJECT2(Class, Base1, Base2) \
  mo = new MetaObjectImpl<Class, Base1, Base2>; \
  mo->setClassName( QLatin1String( #Class ) ); \
  MO_ADD_BASECLASS( Base1 ) \
  MO_ADD_BASECLASS( Base2 ) \
  addMetaObject(mo);

#define MO_ADD_PROPERTY(Class, Type, Getter, Setter) \
  mo->addProperty( new MetaPropertyImpl<Class, Type>( \
    QLatin1String( #Getter ), \
    &Class::Getter, \
    static_cast<void (Class::*)(Type)>(&Class::Setter) ) \
  );

#define MO_ADD_PROPERTY_CR(Class, Type, Getter, Setter) \
  mo->addProperty( new MetaPropertyImpl<Class, Type, const Type&>( \
    QLatin1String( #Getter ), \
    &Class::Getter, \
    static_cast<void (Class::*)(const Type&)>(&Class::Setter) ) \
  );

#define MO_ADD_PROPERTY_RO(Class, Type, Getter) \
  mo->addProperty( new MetaPropertyImpl<Class, Type>( \
    QLatin1String( #Getter ), \
    &Class::Getter ) );

Q_DECLARE_METATYPE(Qt::InputMethodHints)
Q_DECLARE_METATYPE(Qt::MouseButtons)
Q_DECLARE_METATYPE(QGraphicsEffect*)
Q_DECLARE_METATYPE(QGraphicsItemGroup*)
Q_DECLARE_METATYPE(QGraphicsObject*)
Q_DECLARE_METATYPE(QGraphicsWidget*)
Q_DECLARE_METATYPE(QGraphicsItem::CacheMode)
Q_DECLARE_METATYPE(QGraphicsItem::GraphicsItemFlags)

using namespace GammaRay;

namespace GammaRay {
class StaticMetaObjectRepository : public MetaObjectRepository
{
public:
  StaticMetaObjectRepository() : MetaObjectRepository() {}
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
}

void MetaObjectRepository::initQObjectTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT0( QObject );
  MO_ADD_PROPERTY_RO(QObject, bool, signalsBlocked); // TODO setter has non-void return type

  MO_ADD_METAOBJECT1( QWidget, QObject );
  MO_ADD_PROPERTY_RO(QWidget, QWidget*, focusProxy );
}

void MetaObjectRepository::initGraphicsViewTypes()
{
  MetaObject *mo = 0; //createMetaObject( "QGraphicsItem" );
  MO_ADD_METAOBJECT0( QGraphicsItem );
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptDrops,               setAcceptDrops);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptHoverEvents,         setAcceptHoverEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptTouchEvents,         setAcceptTouchEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::MouseButtons,                 acceptedMouseButtons,      setAcceptedMouseButtons);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           boundingRect);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            boundingRegionGranularity, setBoundingRegionGranularity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem::CacheMode,         cacheMode);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           childrenBoundingRect);
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
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isObscured);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isPanel);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isSelected,                setSelected);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isUnderMouse);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isVisible,                 setVisible);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWidget);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWindow);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            opacity,                   setOpacity);
  // TODO: panel, panelModality
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   parentItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsObject*,                 parentObject);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 parentWidget);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          pos,                       setPos);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            rotation,                  setRotation);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            scale,                     setScale);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           sceneBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPointF,                          scenePos);
  // TODO: sceneTransform, shape (and anything else QPainterPath-based)
  MO_ADD_PROPERTY_CR(QGraphicsItem, QString,                          toolTip,                   setToolTip);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   topLevelItem);
  // TODO topLevelWidget, transform
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          transformOriginPoint,     setTransformOriginPoint);
  MO_ADD_PROPERTY_RO(QGraphicsItem, int,                              type);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 window);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            x,                         setX);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            y,                         setY);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            zValue,                    setZValue);

  MO_ADD_METAOBJECT1( QAbstractGraphicsShapeItem, QGraphicsItem );
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QBrush, brush, setBrush);
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QPen,   pen,   setPen);

  MO_ADD_METAOBJECT1( QGraphicsEllipseItem, QAbstractGraphicsShapeItem );
  MO_ADD_PROPERTY_CR(QGraphicsEllipseItem, QRectF, rect,    setRect);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, spanAngle,  setSpanAngle);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, startAngle, setStartAngle);

  // TODO: path, polygon, simple text

  MO_ADD_METAOBJECT1( QGraphicsRectItem, QAbstractGraphicsShapeItem );
  MO_ADD_PROPERTY_CR(QGraphicsRectItem, QRectF, rect, setRect);

  // TODO: line, pixmap

  // no extra properties, but we need the inheritance connection for anything above to work
  MO_ADD_METAOBJECT2( QGraphicsObject, QGraphicsItem, QObject );
}

MetaObjectRepository* MetaObjectRepository::instance()
{
  return s_instance();
}

void MetaObjectRepository::addMetaObject(MetaObject* mo)
{
  Q_ASSERT(!mo->className().isEmpty());
  m_metaObjects.insert(mo->className(), mo);
}

MetaObject* MetaObjectRepository::metaObject(const QString& typeName) const
{
  return m_metaObjects.value(typeName);
}

bool MetaObjectRepository::hasMetaObject(const QString& typeName) const
{
  return m_metaObjects.contains(typeName);
}
