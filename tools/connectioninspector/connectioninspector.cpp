#include "connectioninspector.h"
#include "ui_connectioninspector.h"

#include <connectionfilterproxymodel.h>
#include <probeinterface.h>

using namespace Endoscope;

ConnectionInspector::ConnectionInspector(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::ConnectionInspector )
{
  ui->setupUi( this );

  QSortFilterProxyModel *connectionFilterProxy = new ConnectionFilterProxyModel( this );
  connectionFilterProxy->setSourceModel( probe->connectionModel() );
  ui->connectionSearchLine->setProxy( connectionFilterProxy );
  ui->connectionView->setModel( connectionFilterProxy );
}

#include "connectioninspector.moc"
