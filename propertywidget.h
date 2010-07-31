#ifndef ENDOSCOPE_PROPERTYWIDGET_H
#define ENDOSCOPE_PROPERTYWIDGET_H

#include "ui_propertywidget.h"

namespace Endoscope {

class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class ObjectClassInfoModel;
class ObjectMethodModel;

class PropertyWidget : public QWidget
{
  Q_OBJECT
  public:
    PropertyWidget( QWidget *parent = 0 );

    void setObject( QObject *object );

  private:
    Ui::PropertyWidget ui;
    ObjectStaticPropertyModel *m_staticPropertyModel;
    ObjectDynamicPropertyModel *m_dynamicPropertyModel;
    ObjectClassInfoModel *m_classInfoModel;
    ObjectMethodModel *m_methodModel;
};

}

#endif // ENDOSCOPE_PROPERTYWIDGET_H
