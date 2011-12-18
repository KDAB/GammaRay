#ifndef GAMMARAY_METAPROPERTY_H
#define GAMMARAY_METAPROPERTY_H

#include <QString>
#include <QVariant>

namespace GammaRay {

class MetaObject;

/** Introspectable adaptor to non-QObject properties. */
class MetaProperty
{
public:
  MetaProperty();
  virtual ~MetaProperty();

  /// User-readable name of that property
  virtual QString name() const = 0;

  /// Current value of the property for object @p object.
  virtual QVariant value(void *object) const = 0;

  /// Returns @c true if this property is read-only.
  virtual bool isReadOnly() const = 0;

  /// Allows changing the property value, assuming it's not read-only, for the instance @p object.
  virtual void setValue(void *object, const QVariant &value) = 0;

  /// Returns the name of the data type of this property.
  virtual QString typeName() const = 0;

  /// Returns the class this property belongs to.
  MetaObject* metaObject() const;

private:
  friend class MetaObject;
  void setMetaObject( MetaObject* om );

  MetaObject* m_class;
};


/** Template-ed implementation of MetaProperty. */
template <typename Class, typename ValueType, typename SetterArgType = ValueType>
class MetaPropertyImpl : public MetaProperty
{
public:
  inline MetaPropertyImpl( const QString &name, ValueType (Class::*getter)() const, void (Class::*setter)(SetterArgType) = 0 ) :
    m_name( name ),
    m_getter( getter ),
    m_setter( setter )
  {
  }

  inline QString name() const { return m_name; }
  inline bool isReadOnly() const { return m_setter == 0 ; }
  inline QVariant value(void *object) const { return value(static_cast<Class*>(object)); }
  inline void setValue(void *object, const QVariant &value) { setValue(static_cast<Class*>(object), value); }

private:
  inline QVariant value(Class *object) const
  {
    const ValueType v = (object->*(m_getter))();
    return QVariant::fromValue(v);
  }

  inline void setValue(Class *object, const QVariant &value)
  {
    if (isReadOnly())
      return;
    (object->*(m_setter))(value.value<ValueType>());
  }

  inline QString typeName() const
  {
    return QMetaType::typeName( qMetaTypeId<ValueType>() );
  }

private:
  QString m_name;
  ValueType (Class::*m_getter)() const;
  void (Class::*m_setter)(SetterArgType);
};

}

#endif
