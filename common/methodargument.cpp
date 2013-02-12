#include "methodargument.h"

#include <QSharedData>
#include <QMetaType>

using namespace GammaRay;

class GammaRay::MethodArgumentPrivate : public QSharedData
{
  public:
    MethodArgumentPrivate() : QSharedData(), data(0) {}

    MethodArgumentPrivate(const MethodArgumentPrivate &other) : QSharedData(other)
    {
      value = other.value;
      name = other.name;
      data = 0;
    }

    ~MethodArgumentPrivate()
    {
      if (data)
        QMetaType::destroy(value.type(), data);
    }

    QVariant value;
    QByteArray name;
    void *data;
};

MethodArgument::MethodArgument() : d(new MethodArgumentPrivate)
{
}

MethodArgument::MethodArgument(const QVariant& v) : d(new MethodArgumentPrivate)
{
  d->value = v;
  d->name = v.typeName();
}

MethodArgument::MethodArgument(const MethodArgument& other) : d(other.d)
{
}

MethodArgument::~MethodArgument()
{
}

MethodArgument& MethodArgument::operator=(const MethodArgument& other)
{
  d = other.d;
  return *this;
}

MethodArgument::operator QGenericArgument() const
{
  if (d->value.isValid()) {
    d->data = QMetaType::construct(d->value.userType(), d->value.constData());
    Q_ASSERT(d->data);
    return QGenericArgument(d->name.data(), d->data);
  }
  return QGenericArgument();
}
