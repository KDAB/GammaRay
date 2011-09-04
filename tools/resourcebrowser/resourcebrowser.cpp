#include "resourcebrowser.h"
#include "ui_resourcebrowser.h"

#include <qt/resourcemodel.h>

using namespace Endoscope;

ResourceBrowser::ResourceBrowser(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::ResourceBrowser )
{
  Q_UNUSED(probe);
  ui->setupUi( this );

  ResourceModel *resourceModel = new ResourceModel(this);
  ui->treeView->setModel(resourceModel);
  ui->treeView->expandAll();
  connect( ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(resourceSelected(QItemSelection,QItemSelection)));
}

void ResourceBrowser::resourceSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected)
  const QModelIndex selectedRow = selected.first().topLeft();
  const QFileInfo fi(selectedRow.data( ResourceModel::FilePathRole ).toString());

  if ( fi.isFile() ) {
    const QStringList l = QStringList() << "jpg" << "png" << "jpeg";
    if ( l.contains( fi.suffix() ) ) {
      ui->label_3->setPixmap( fi.absoluteFilePath() );
      ui->stackedWidget->setCurrentWidget(ui->page_4);
    } else {
      QFile f( fi.absoluteFilePath() );
      f.open(QFile::ReadOnly | QFile::Text);
      ui->textBrowser->setText( f.readAll() );
      ui->stackedWidget->setCurrentWidget(ui->page_3);
    }
  }

}

#include "resourcebrowser.moc"
