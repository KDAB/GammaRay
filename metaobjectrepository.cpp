#include "metaobjectrepository.h"
#include "metaobject.h"

#include <qglobal.h>
#include <qgraphicsitem.h>
#include <qpen.h>
#include <qobject.h>
#include <qwidget.h>

#define MO_ADD_PROPERTY(Class, Type, Getter, Setter) \
  mo->addProperty( new MetaPropertyImpl<Class, Type>( \
    QLatin1String( #Getter ), \
    &Class::Getter, \
    static_cast<void (Class::*)(Type)>(&Class::Setter) ) \
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
  MetaObject *mo = createMetaObject( "QObject" );
  MO_ADD_PROPERTY_RO(QObject, bool, signalsBlocked); // TODO setter has non-void return type

  mo = createMetaObject( "QWidget", "QObject" );
  MO_ADD_PROPERTY_RO(QWidget, QWidget*, focusProxy );
}

void MetaObjectRepository::initGraphicsViewTypes()
{
  MetaObject *mo = createMetaObject( "QGraphicsItem" );
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptDrops,               setAcceptDrops);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptHoverEvents,         setAcceptHoverEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptTouchEvents,         setAcceptTouchEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::MouseButtons,                 acceptedMouseButtons,      setAcceptedMouseButtons);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           boundingRect);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            boundingRegionGranularity, setBoundingRegionGranularity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem::CacheMode,         cacheMode);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           childrenBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QCursor,                          cursor); // TODO setter
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
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPointF,                          pos); // TODO support const ref types for setter arguments
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            rotation,                  setRotation);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            scale,                     setScale);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           sceneBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPointF,                          scenePos);
  // TODO: sceneTransform, shape (and anything else QPainterPath-based)
//  MO_ADD_PROPERTY   (QGraphicsItem, QString,                          toolTip,                 setToolTip); TODO const ref
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   topLevelItem);
  // TODO topLevelWidget, transform
//  MO_ADD_PROPERTY   (QGraphicsItem, QPointF,                          transformOriginPoint, setTransformOriginPoint); TODO cost ref
  MO_ADD_PROPERTY_RO(QGraphicsItem, int,                              type);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 window);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            x,                         setX);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            y,                         setY);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            zValue,                    setZValue);

  mo = createMetaObject( "QAbstractGraphicsShapeItem", "QGraphicsItem" );
  MO_ADD_PROPERTY_RO(QAbstractGraphicsShapeItem, QBrush, brush); // TODO setter
  MO_ADD_PROPERTY_RO(QAbstractGraphicsShapeItem, QPen,   pen); //, setPen); TODO const ref

  mo = createMetaObject( "QGraphicsEllipseItem", "QAbstractGraphicsShapeItem" );
  MO_ADD_PROPERTY_RO(QGraphicsEllipseItem, QRectF, rect) // TODO setter
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, spanAngle, setSpanAngle);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, startAngle, setStartAngle);

  // TODO: path, polygon, simple text

  mo = createMetaObject( "QGraphicsRectItem", "QAbstractGraphicsShapeItem" );
  MO_ADD_PROPERTY_RO(QGraphicsRectItem, QRectF, rect); // TODO setter

  // TODO: line, object, pixmap
  // TODO multi-inheritance support for e.g. QGraphicsObject

}

MetaObjectRepository* MetaObjectRepository::instance()
{
  return s_instance();
}

void MetaObjectRepository::addMetaObject(const QString &typeName, MetaObject* objectType)
{
    m_metaObjects.insert(typeName, objectType);
}

MetaObject* MetaObjectRepository::metaObject(const QString& typeName) const
{
  return m_metaObjects.value(typeName);
}

MetaObject* MetaObjectRepository::createMetaObject(const QString& typeName, const QString& baseClassName)
{
  Q_ASSERT( !typeName.isEmpty() );
  Q_ASSERT( baseClassName != typeName );
    MetaObject* mo = new MetaObject;
  mo->setClassName( typeName );
    addMetaObject( typeName, mo );
  if ( !baseClassName.isEmpty() )
    mo->setBaseClass( metaObject( baseClassName ) );
  return mo;
}

MetaObject* MetaObjectRepository::createMetaObject(const char* typeName, const char* baseClassName)
{
  return createMetaObject( QLatin1String(typeName), QLatin1String(baseClassName) );
}

bool MetaObjectRepository::hasMetaObject(const QString& typeName) const
{
  return m_metaObjects.contains(typeName);
}
