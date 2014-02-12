#ifndef PROPERTYCONTROLLEREXTENSION_H
#define PROPERTYCONTROLLEREXTENSION_H

class QObject;
class QMetaObject;

namespace GammaRay {

class PropertyController;

class PropertyControllerExtension
{
public:
  explicit PropertyControllerExtension() {}

  /** Sets the object that should be represented by this extension. */
  virtual void setObject(void *object, const QString &typeName) = 0;
  virtual void setObject(QObject *object) = 0;
  virtual void setMetaObject(const QMetaObject *metaObject) = 0;
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
