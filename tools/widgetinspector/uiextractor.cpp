#include "uiextractor.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>

using namespace GammaRay;

bool UiExtractor::checkProperty(QObject* obj, const QString& prop) const
{
  const QMetaObject *mo = obj->metaObject();
  const QMetaProperty mp = mo->property(mo->indexOfProperty(prop.toLatin1()));

  // TODO come up with some more aggressive filtering
  if (mp.isValid() && mp.isDesignable(obj) && mp.isStored(obj) && mp.isWritable()) {
    const QVariant value = mp.read(obj);
    if (value.isNull() || !value.isValid())
      return false;

    // ### this assumes empty == default, maybe we can temporarily reset and compare actual and default values instead?
    if (value.type() == QVariant::String)
      return !value.toString().isEmpty();

    return true;
  }

  return false;
}
