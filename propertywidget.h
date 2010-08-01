#ifndef ENDOSCOPE_PROPERTYWIDGET_H
#define ENDOSCOPE_PROPERTYWIDGET_H

#include "ui_propertywidget.h"

namespace Endoscope {

class ConnectionFilterProxyModel;
class ObjectDynamicPropertyModel;
class ObjectStaticPropertyModel;
class ObjectClassInfoModel;
class ObjectMethodModel;

class PropertyWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit PropertyWidget( QWidget *parent = 0 );

    void setObject( QObject *object );

  private slots:
    void methodActivated( const QModelIndex &index );

  private:
    Ui::PropertyWidget ui;
    QWeakPointer<QObject> m_object;
    ObjectStaticPropertyModel *m_staticPropertyModel;
    ObjectDynamicPropertyModel *m_dynamicPropertyModel;
    ObjectClassInfoModel *m_classInfoModel;
    ObjectMethodModel *m_methodModel;
    ConnectionFilterProxyModel *m_inboundConnectionModel;
    ConnectionFilterProxyModel *m_outboundConnectionModel;
};

}

#endif // ENDOSCOPE_PROPERTYWIDGET_H
