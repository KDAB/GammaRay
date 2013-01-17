/*
  fontbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "fontbrowser.h"
#include "ui_fontbrowser.h"

#include "fontmodel.h"

using namespace GammaRay;

FontBrowser::FontBrowser(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::FontBrowser)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  QFontDatabase database;

  m_selectedFontModel = new FontModel(this);
  ui->selectedFontsView->setModel(m_selectedFontModel);
  ui->selectedFontsView->setRootIsDecorated(false);

  ui->fontTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  foreach (const QString &family, database.families()) {
    QTreeWidgetItem *familyItem = new QTreeWidgetItem(ui->fontTree);
    familyItem->setText(0, family);

    foreach (const QString &style, database.styles(family)) {
      QTreeWidgetItem *styleItem = new QTreeWidgetItem(familyItem);
      styleItem->setText(0, style);

      QString sizes;
      foreach (int points, database.smoothSizes(family, style)) {
        sizes += QString::number(points) + ' ';
      }

      styleItem->setText(1, sizes.trimmed());
      styleItem->setToolTip(1, sizes.trimmed());
    }
  }
  ui->fontTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui->selectedFontsView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  connect(ui->fontTree->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(updateFonts(QItemSelection,QItemSelection)));
  connect(ui->fontText, SIGNAL(textChanged(QString)),
          m_selectedFontModel, SLOT(updateText(QString)));

  ui->pointSize->setValue(font().pointSize());

  connect(ui->boldBox, SIGNAL(toggled(bool)),
          m_selectedFontModel, SLOT(toggleBoldFont(bool)));
  connect(ui->italicBox, SIGNAL(toggled(bool)),
          m_selectedFontModel, SLOT(toggleItalicFont(bool)));
  connect(ui->underlineBox, SIGNAL(toggled(bool)),
          m_selectedFontModel, SLOT(toggleUnderlineFont(bool)));
  connect(ui->pointSize, SIGNAL(valueChanged(int)),
          m_selectedFontModel, SLOT(setPointSize(int)));

  // init
  m_selectedFontModel->updateText(ui->fontText->text());
}

void FontBrowser::updateFonts(const QItemSelection &selected, const QItemSelection &deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);
  QList<QFont> previousFonts = m_selectedFontModel->currentFonts();
  QStringList previousFontNames;
  foreach (const QFont &f, previousFonts) {
    previousFontNames.append(f.family());
  }
  QList<QFont> currentFonts;
  QStringList currentFontNames;
  foreach (const QModelIndex &index, ui->fontTree->selectionModel()->selectedRows()) {
    if (index.parent().isValid()) {
      continue;
    }
    QFont font(index.data().toString());
    font.setBold(ui->boldBox->isChecked());
    font.setUnderline(ui->underlineBox->isChecked());
    font.setItalic(ui->italicBox->isChecked());
    font.setPointSize(ui->pointSize->value());
    currentFontNames.append(font.family());
    if (previousFontNames.contains(font.family())) {
      continue;
    }
    currentFonts.append(font);
  }
  {
    QList<QFont>::iterator it = previousFonts.begin();
    while (it != previousFonts.end()) {
      if (!currentFontNames.contains(it->family())) {
        it = previousFonts.erase(it);
      } else {
        ++it;
      }
    }
  }

  currentFonts << previousFonts;
  m_selectedFontModel->updateFonts(currentFonts);
}

#include "fontbrowser.moc"
