#include "codecbrowser.h"
#include "ui_codecbrowser.h"

#include "codecmodel.h"

using namespace Endoscope;

CodecBrowser::CodecBrowser(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::CodecBrowser )
{
  ui->setupUi( this );

  ui->codecList->setModel(new AllCodecsModel(this));
  ui->codecList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_selectedCodecsModel = new SelectedCodecsModel(this);
  ui->selectedCodecs->setModel(m_selectedCodecsModel);

  connect(ui->codecList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateCodecs(QItemSelection,QItemSelection)));
  connect(ui->codecText, SIGNAL(textChanged(QString)), m_selectedCodecsModel, SLOT(updateText(QString)));
}

CodecBrowser::~CodecBrowser()
{
  delete ui;
}

void CodecBrowser::updateCodecs(const QItemSelection& selected, const QItemSelection& deselected)
{
  QStringList previousCodecs = m_selectedCodecsModel->currentCodecs();

  QStringList currentCodecNames;
  foreach(const QModelIndex &index, ui->codecList->selectionModel()->selectedRows()) {
    const QString codecName = index.data().toString();
    if (previousCodecs.contains(codecName)) {
      continue;
    }
    currentCodecNames.append(codecName);
  }

  currentCodecNames << previousCodecs;
  m_selectedCodecsModel->setCodecs(currentCodecNames);
}

#include "codecbrowser.moc"
