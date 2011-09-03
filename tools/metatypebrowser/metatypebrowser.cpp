#include "metatypebrowser.h"
#include "ui_metatypebrowser.h"

#include "metatypesmodel.h"

using namespace Endoscope;

MetaTypeBrowser::MetaTypeBrowser(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::MetaTypeBrowser )
{
  Q_UNUSED(probe);
  ui->setupUi( this );

  MetaTypesModel *mtm = new MetaTypesModel(this);
  ui->metaTypeView->setModel(mtm);
}

#include "metatypebrowser.moc"
