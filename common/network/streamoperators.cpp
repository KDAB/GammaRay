#include "streamoperators.h"

#include <include/metatypedeclarations.h>

#include <QDataStream>
#include <QMetaMethod>

using namespace GammaRay;

QDataStream &operator<<(QDataStream &out, QMetaMethod::MethodType methodType)
{
  out << qint32(methodType);
  return out;
}

QDataStream &operator>>(QDataStream &in, QMetaMethod::MethodType &methodType)
{
  qint32 t;
  in >> t;
  methodType = static_cast<QMetaMethod::MethodType>(t);
  return in;
}


void StreamOperators::registerOperators()
{
  qRegisterMetaTypeStreamOperators<QMetaMethod::MethodType>();
}
