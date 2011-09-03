#include "fontbrowser.h"
#include "ui_fontbrowser.h"

#include "fontmodel.h"

using namespace Endoscope;

FontBrowser::FontBrowser(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui(new Ui::FontBrowser)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  QFontDatabase database;

  m_selectedFontModel = new FontModel(this);
  ui->selectedFontsView->setModel(m_selectedFontModel);

  ui->fontTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  foreach (const QString &family, database.families()) {
      QTreeWidgetItem *familyItem = new QTreeWidgetItem(ui->fontTree);
      familyItem->setText(0, family);

      foreach (const QString &style, database.styles(family)) {
          QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
          styleItem->setText(0, style);

          QString sizes;
          foreach (int points, database.smoothSizes(family, style))
              sizes += QString::number(points) + ' ';

          styleItem->setText(1, sizes.trimmed());
      }
  }
  connect(ui->fontTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateFonts(QItemSelection,QItemSelection)));
  connect(ui->fontText, SIGNAL(textChanged(QString)), m_selectedFontModel, SLOT(updateText(QString)));
}

FontBrowser::~FontBrowser()
{
  delete ui;
}

void FontBrowser::updateFonts(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);
  QList<QFont> previousFonts = m_selectedFontModel->currentFonts();
  QStringList previousFontNames;
  foreach(const QFont &f, previousFonts) {
    previousFontNames.append(f.family());
  }
  QList<QFont> currentFonts;
  QStringList currentFontNames;
  foreach(const QModelIndex &index, ui->fontTree->selectionModel()->selectedRows()) {
    if (index.parent().isValid())
      continue;
    QFont font(index.data().toString());
    currentFontNames.append(font.family());
    if (previousFontNames.contains(font.family())) {
      continue;
    }
    currentFonts.append(font);
  }
  {
    QList<QFont>::iterator it = previousFonts.begin();
    while ( it != previousFonts.end()) {
      if (!currentFontNames.contains(it->family()))
        it = previousFonts.erase(it);
      else
        ++it;
    }
  }

  currentFonts << previousFonts;
  m_selectedFontModel->updateFonts(currentFonts);
}

#include "fontbrowser.moc"
