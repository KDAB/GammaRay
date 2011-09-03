#include "webinspector.h"
#include "ui_webinspector.h"
#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>
#include <probeinterface.h>
#include <objectlistmodel.h>

using namespace Endoscope;

WebInspector::WebInspector(ProbeInterface *probe, QWidget* parent):
  QWidget( parent ),
  ui( new Ui::WebInspector )
{
  ui->setupUi( this );

  ObjectTypeFilterProxyModel<QWebPage> *webPageFilter = new ObjectTypeFilterProxyModel<QWebPage>( this );
  webPageFilter->setSourceModel( probe->objectListModel() );
  SingleColumnObjectProxyModel* singleColumnProxy = new SingleColumnObjectProxyModel( this );
  singleColumnProxy->setSourceModel( webPageFilter );
  ui->webPageComboBox->setModel( singleColumnProxy );
  connect( ui->webPageComboBox, SIGNAL(activated(int)), SLOT(webPageSelected(int)) );
}

void WebInspector::webPageSelected(int index)
{
  QObject* obj = ui->webPageComboBox->itemData( index, ObjectListModel::ObjectRole ).value<QObject*>();
  QWebPage *page = qobject_cast<QWebPage*>( obj );
  if ( page ) {
    page->settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
    ui->webInspector->setPage( page );
  }
}

#include "webinspector.moc"
