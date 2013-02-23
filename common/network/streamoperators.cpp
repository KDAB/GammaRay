#include "streamoperators.h"
#include "enums.h"

#include <include/metatypedeclarations.h>

#include <QDataStream>
#include <QMetaMethod>

using namespace GammaRay;

#define MAKE_ENUM_OPERATORS(enumType) \
  QDataStream &operator<<(QDataStream &out, enumType value) \
  { \
    out << qint32(value); \
    return out; \
  } \
  \
  QDataStream &operator>>(QDataStream &in, enumType &value) \
  { \
    qint32 t; \
    in >> t; \
    value = static_cast<enumType>(t); \
    return in; \
  }

MAKE_ENUM_OPERATORS(QMetaMethod::MethodType)
MAKE_ENUM_OPERATORS(PropertyWidgetDisplayState::State)
MAKE_ENUM_OPERATORS(Qt::ConnectionType)

void StreamOperators::registerOperators()
{
  qRegisterMetaTypeStreamOperators<QMetaMethod::MethodType>();
  qRegisterMetaTypeStreamOperators<PropertyWidgetDisplayState::State>();
  qRegisterMetaTypeStreamOperators<Qt::ConnectionType>();
}
