#include "propertywidget.h"
#include "objectstaticpropertymodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectclassinfomodel.h"
#include "objectmethodmodel.h"
#include "connectionmodel.h"
#include "connectionfilterproxymodel.h"
#include "probe.h"
#include "methodinvocationdialog.h"

#include <KDebug>

using namespace Endoscope;

PropertyWidget::PropertyWidget(QWidget* parent) :
  QWidget(parent),
  m_staticPropertyModel( new ObjectStaticPropertyModel( this ) ),
  m_dynamicPropertyModel( new ObjectDynamicPropertyModel( this ) ),
  m_classInfoModel( new ObjectClassInfoModel( this ) ),
  m_methodModel( new ObjectMethodModel( this ) ),
  m_inboundConnectionModel( new ConnectionFilterProxyModel( this ) ),
  m_outboundConnectionModel( new ConnectionFilterProxyModel( this ) )
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

  proxy = new QSortFilterProxyModel( this );
  proxy->setSourceModel( m_methodModel );
  ui.methodView->setModel( proxy );
  ui.methodSearchLine->setProxy( proxy );
  connect( ui.methodView, SIGNAL(doubleClicked(QModelIndex)), SLOT(methodActivated(QModelIndex)) );

  proxy = new QSortFilterProxyModel( this );
  proxy->setSourceModel( m_classInfoModel );
  ui.classInfoView->setModel( proxy );
  ui.classInfoSearchLine->setProxy( proxy );

  m_inboundConnectionModel->setSourceModel( Probe::instance()->connectionModel() );
  ui.inboundConnectionView->setModel( m_inboundConnectionModel );
  ui.inboundConnectionSearchLine->setProxy( m_inboundConnectionModel );

  m_outboundConnectionModel->setSourceModel( Probe::instance()->connectionModel() );
  ui.outboundConnectionView->setModel( m_outboundConnectionModel );
  ui.outboundConnectionSearchLine->setProxy( m_outboundConnectionModel );
}

void Endoscope::PropertyWidget::setObject(QObject* object)
{
  m_object = object;
  m_staticPropertyModel->setObject( object );
  m_dynamicPropertyModel->setObject( object );
  m_classInfoModel->setObject( object );
  m_methodModel->setObject( object );
  m_inboundConnectionModel->filterReceiver( object );
  m_outboundConnectionModel->filterSender( object );
}

void PropertyWidget::methodActivated(const QModelIndex& index)
{
  const QMetaMethod method = index.data( ObjectMethodModel::MetaMethodRole ).value<QMetaMethod>();
  if ( method.methodType() == QMetaMethod::Slot ) {
    kDebug() << "invoking" << method.signature();
    MethodInvocationDialog *dlg = new MethodInvocationDialog( this );
    dlg->setMethod( m_object.data(), method );
    dlg->show();
  } else if ( method.methodType() == QMetaMethod::Signal ) {
    kDebug() << "connecting to" << method.signature();
  }
}


#include "propertywidget.moc"
