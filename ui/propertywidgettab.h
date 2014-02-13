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
    virtual const QString &name() const = 0;
    virtual const QString &label() const = 0;
};

template <typename T>
class PropertyWidgetTabFactory : public PropertyWidgetTabFactoryBase
{
  public:
    explicit PropertyWidgetTabFactory(const QString &name, const QString &label)
      : m_name(name),
      m_label(label)
    {
    }
    QWidget *createWidget(PropertyWidget *parent)
    {
      return new T(parent);
    }
    const QString &name() const
    {
      return m_name;
    }
    const QString &label() const
    {
      return m_label;
    }
  private:
    QString m_name;
    QString m_label;
};

}

#endif // PROPERTYWIDGETTAB_H
