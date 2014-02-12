#ifndef PROPERTYWIDGETTAB_H
#define PROPERTYWIDGETTAB_H

#include <QWidget>
#include <QString>

namespace GammaRay {

class PropertyWidget;

class PropertyWidgetTabFactoryBase
{
  public:
    explicit PropertyWidgetTabFactoryBase() {}
    virtual QWidget *createWidget(PropertyWidget *parent) = 0;
    virtual QString name() = 0;
};

template <typename T>
class PropertyWidgetTabFactory : public PropertyWidgetTabFactoryBase
{
  public:
    explicit PropertyWidgetTabFactory(QString name)
    {
      m_name = name;
    }
    QWidget *createWidget(PropertyWidget *parent)
    {
      return new T(parent);
    }
    QString name()
    {
      return m_name;
    }
  private:
    QString m_name;
};

}

#endif // PROPERTYWIDGETTAB_H
