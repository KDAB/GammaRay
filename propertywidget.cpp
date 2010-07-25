#include "propertywidget.h"
#include "objectstaticpropertymodel.h"
#include "objectdynamicpropertymodel.h"
#include <QtGui/QSortFilterProxyModel>

using namespace Endoscope;

PropertyWidget::PropertyWidget(QWidget* parent) :
  QWidget(parent),
  m_staticPropertyModel( new ObjectStaticPropertyModel( this ) ),
  m_dynamicPropertyModel( new ObjectDynamicPropertyModel( this ) )
{
  ui.setupUi( this );

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel( this );
  proxy->setSourceModel( m_staticPropertyModel );
  ui.staticPropertyView->setModel( proxy );
  ui.staticPropertySearchLine->setProxy( proxy );

  proxy = new QSortFilterProxyModel( this );
  proxy->setSourceModel( m_dynamicPropertyModel );
  ui.dynamicPropertyView->setModel( proxy );
  ui.dynamicPropertySearchLine->setProxy( proxy );
}

void Endoscope::PropertyWidget::setObject(QObject* object)
{
  m_staticPropertyModel->setObject( object );
  m_dynamicPropertyModel->setObject( object );
}

#include "propertywidget.h"
