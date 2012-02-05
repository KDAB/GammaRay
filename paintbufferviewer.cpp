#include "paintbufferviewer.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include "ui_paintbufferviewer.h"

#include "paintbuffermodel.h"

using namespace GammaRay;

PaintBufferViewer::PaintBufferViewer(QWidget* parent): QWidget(parent), ui(new Ui::PaintBufferViewer), m_bufferModel(new PaintBufferModel(this))
{
  ui->setupUi(this);

  ui->commandView->setModel(m_bufferModel);
  connect(ui->commandView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(commandSelected()));
}

PaintBufferViewer::~PaintBufferViewer()
{
}

void PaintBufferViewer::setPaintBuffer(const QPaintBuffer& buffer)
{
  m_buffer = buffer;
  m_bufferModel->setPaintBuffer(buffer);
  ui->replayWidget->setPaintBuffer(buffer);
  commandSelected();
}

void PaintBufferViewer::commandSelected()
{
  if (!ui->commandView->selectionModel()->hasSelection()) {
    ui->replayWidget->setEndCommandIndex(m_bufferModel->rowCount());
    return;
  }

  const QModelIndex index = ui->commandView->selectionModel()->selectedRows().first();
  ui->replayWidget->setEndCommandIndex(index.row() + 1); // include the current row
}

#include "paintbufferviewer.moc"
#endif
