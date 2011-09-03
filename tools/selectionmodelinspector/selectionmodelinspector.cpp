#include "selectionmodelinspector.h"
#include "ui_selectionmodelinspector.h"

#include <objecttypefilterproxymodel.h>
#include <probeinterface.h>

using namespace Endoscope;

SelectionModelInspector::SelectionModelInspector(ProbeInterface* probe, QWidget* widget):
  QWidget(widget),
  ui( new Ui::SelectionModelInspector )
{
  ui->setupUi( this );

  ObjectTypeFilterProxyModel<QItemSelectionModel> *selectionModelProxy = new ObjectTypeFilterProxyModel<QItemSelectionModel>( this );
  selectionModelProxy->setSourceModel( probe->objectListModel() );
  ui->selectionModelView->setModel(selectionModelProxy);
  connect(ui->selectionModelView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(selectionModelSelected(QItemSelection,QItemSelection)));
}

void SelectionModelInspector::selectionModelSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected);
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *selectionModelObject = selectedRow.data( ObjectListModel::ObjectRole ).value<QObject*>();
  QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel*>(selectionModelObject);
  if (selectionModel && selectionModel->model()) {
    ui->selectionModelVisualizer->setModel(const_cast<QAbstractItemModel*>(selectionModel->model()));
    ui->selectionModelVisualizer->setSelectionModel(selectionModel);
  }
}

#include "selectionmodelinspector.moc"
