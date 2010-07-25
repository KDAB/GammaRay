#ifndef ENDOSCOPE_PROPERTYWIDGET_H
#define ENDOSCOPE_PROPERTYWIDGET_H

#include "ui_propertywidget.h"

namespace Endoscope {

class ObjectPropertyModel;


class PropertyWidget : public QWidget
{
  Q_OBJECT
  public:
    PropertyWidget( QWidget *parent = 0 );

    void setObject( QObject *object );

  private:
    Ui::PropertyWidget ui;
    ObjectPropertyModel *m_staticPropertyModel;
};

}

#endif // ENDOSCOPE_PROPERTYWIDGET_H
