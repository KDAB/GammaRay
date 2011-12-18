#ifndef GAMMARAY_METAOBJECT_H
#define GAMMARAY_METAOBJECT_H

#include "metaproperty.h"

#include <QVector>

namespace GammaRay {

/** Compile-time introspection adaptor for non-QObject classes. */
class MetaObject
{
public:
    MetaObject();
  virtual ~MetaObject();

  /** Returns the amount of properties available in this class (including base classes). */
  int propertyCount() const;
  /** Returns the property adaptor for index @p index. */
  MetaProperty* propertyAt(int index) const;

  /** Set base class adaptor. */
  void setBaseClass( MetaObject* baseClass );
  /** Add a property for this class. This transfers ownership. */
  void addProperty( MetaProperty* property );

  /// Returns the name of the class represented by this object.
  QString className() const;

private:
  friend class MetaObjectRepository;
  void setClassName( const QString &className );

private:
  QVector<MetaProperty*> m_properties;
  MetaObject* m_baseClass;
  QString m_className;
};

}

#endif // GAMMARAY_METAOBJECT_H
