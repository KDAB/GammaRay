/*
  fontbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "fontmodel.h"
#include <network/objectbroker.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QFontDatabase>

using namespace GammaRay;

FontBrowser::FontBrowser(ProbeInterface *probe, QObject *parent)
  : ObjectServer("com.kdab.GammaRay.FontBrowser", parent)
  , m_selectedFontModel(new FontModel(this))
{
  subscribeToSignal("updateText", m_selectedFontModel, "updateText");
  subscribeToSignal("toggleBoldFont", m_selectedFontModel, "toggleBoldFont");
  subscribeToSignal("toggleItalicFont", m_selectedFontModel, "toggleItalicFont");
  subscribeToSignal("toggleUnderlineFont", m_selectedFontModel, "toggleUnderlineFont");
  subscribeToSignal("setPointSize", m_selectedFontModel, "setPointSize");

  QStandardItemModel *model = new QStandardItemModel(this);
  model->setHorizontalHeaderLabels(QStringList() << tr("Fonts") << tr("Smooth sizes"));
  QFontDatabase database;
  foreach (const QString &family, database.families()) {
    QStandardItem *familyItem = new QStandardItem;
    familyItem->setText(family);

    foreach (const QString &style, database.styles(family)) {
      QStandardItem *styleItem0 = new QStandardItem;
      styleItem0->setText(style);

      QString sizes;
      foreach (int points, database.smoothSizes(family, style)) {
        sizes += QString::number(points) + ' ';
      }

      QStandardItem *styleItem1 = new QStandardItem;
      styleItem1->setText(sizes.trimmed());
      styleItem1->setToolTip(sizes.trimmed());

      familyItem->appendRow(QList<QStandardItem*>() << styleItem0 << styleItem1);
    }

    model->appendRow(familyItem);
  }

  probe->registerModel("com.kdab.GammaRay.FontModel", model);
  m_fontSelectionModel = ObjectBroker::selectionModel(model);
  connect(m_fontSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(updateFonts()));
  probe->registerModel("com.kdab.GammaRay.SelectedFontModel", m_selectedFontModel);
}

void FontBrowser::updateFonts()
{
  QList<QFont> previousFonts = m_selectedFontModel->currentFonts();
  QStringList previousFontNames;
  foreach (const QFont &f, previousFonts) {
    previousFontNames.append(f.family());
  }
  QList<QFont> currentFonts;
  QStringList currentFontNames;
  foreach (const QModelIndex &index, m_fontSelectionModel->selectedRows()) {
    if (index.parent().isValid()) {
      continue;
    }
    QFont font(index.data().toString());
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
