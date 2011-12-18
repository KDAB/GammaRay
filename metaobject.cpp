#include "metaobject.h"

using namespace GammaRay;

MetaObject::MetaObject() : m_baseClass(0)
{
}

MetaObject::~MetaObject()
{
  qDeleteAll(m_properties);
}

int MetaObject::propertyCount() const
{
  if ( !m_baseClass )
    return m_properties.size();
  return m_properties.size() + m_baseClass->propertyCount();
}

MetaProperty* MetaObject::propertyAt(int index) const
{
  Q_ASSERT( index >= 0 );
  if ( index >= m_properties.size() ) {
    Q_ASSERT( m_baseClass );
    return m_baseClass->propertyAt(index - m_properties.size());
  }
  return m_properties.at(index);
}

void MetaObject::setBaseClass(MetaObject* baseClass)
{
  m_baseClass = baseClass;
}

void MetaObject::addProperty(MetaProperty* property)
{
  // TODO: sort
  property->setMetaObject(this);
  m_properties.push_back(property);
}

QString MetaObject::className() const
{
  return m_className;
}

void MetaObject::setClassName(const QString& className)
{
  m_className = className;
}
