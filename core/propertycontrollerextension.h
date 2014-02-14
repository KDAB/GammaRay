#ifndef PROPERTYCONTROLLEREXTENSION_H
#define PROPERTYCONTROLLEREXTENSION_H

class QObject;
class QMetaObject;

#include "gammaray_core_export.h"
#include <QString>

namespace GammaRay {

class PropertyController;

class GAMMARAY_CORE_EXPORT PropertyControllerExtension
{
public:
  explicit PropertyControllerExtension(const QString &name);

  /** Sets the object that should be represented by this extension. */
  virtual bool setObject(void *object, const QString &typeName) = 0;
  virtual bool setObject(QObject *object) = 0;
  virtual bool setMetaObject(const QMetaObject *metaObject) = 0;

  const QString &name() const;

private:
  QString m_name;
};

class PropertyControllerExtensionFactoryBase {
  public:
    explicit PropertyControllerExtensionFactoryBase() {}
    virtual PropertyControllerExtension *create(PropertyController *controller) = 0;
};

template <typename T>
class PropertyControllerExtensionFactory : public PropertyControllerExtensionFactoryBase
{
  public:
    explicit PropertyControllerExtensionFactory() {}
    PropertyControllerExtension *create(PropertyController *controller)
    {
      return new T(controller);
    }
};

}

#endif // PROPERTYCONTROLLEREXTENSION_H
