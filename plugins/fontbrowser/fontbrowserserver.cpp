/*
  fontbrowserserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fontbrowserserver.h"

#include "fontmodel.h"

#include <common/objectbroker.h>
#include <core/probeinterface.h>

#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QFontDatabase>

using namespace GammaRay;

FontBrowserServer::FontBrowserServer(ProbeInterface *probe, QObject *parent)
  : FontBrowserInterface(parent)
  , m_selectedFontModel(new FontModel(this))
{
  QStandardItemModel *model = new QStandardItemModel(this);
  model->setHorizontalHeaderLabels(QStringList() << tr("Fonts") << tr("Smooth sizes"));
  QFontDatabase database;
  foreach (const QString &family, database.families()) {
    QStandardItem *familyItem = new QStandardItem;
    familyItem->setText(family);
    familyItem->setEditable(false);
    familyItem->setData(QFont(family));

    foreach (const QString &style, database.styles(family)) {
      QStandardItem *styleItem0 = new QStandardItem;
      styleItem0->setText(style);
      styleItem0->setEditable(false);
      styleItem0->setData(database.font(family, style, 10));

      QString sizes;
      foreach (int points, database.smoothSizes(family, style)) {
        sizes += QString::number(points) + ' ';
      }

      QStandardItem *styleItem1 = new QStandardItem;
      styleItem1->setText(sizes.trimmed());
      styleItem1->setEditable(false);
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

void FontBrowserServer::updateFonts()
{
  QList<QFont> currentFonts;
  foreach (const QModelIndex &index, m_fontSelectionModel->selectedRows()) {
    currentFonts << index.data(Qt::UserRole + 1).value<QFont>();
  }
  m_selectedFontModel->updateFonts(currentFonts);
}

void FontBrowserServer::setPointSize(int size)
{
  m_selectedFontModel->setPointSize(size);
}

void FontBrowserServer::toggleBoldFont(bool bold)
{
  m_selectedFontModel->toggleBoldFont(bold);
}

void FontBrowserServer::toggleItalicFont(bool italic)
{
  m_selectedFontModel->toggleItalicFont(italic);
}

void FontBrowserServer::toggleUnderlineFont(bool underline)
{
  m_selectedFontModel->toggleUnderlineFont(underline);
}

void FontBrowserServer::updateText(const QString &text)
{
  m_selectedFontModel->updateText(text);
}

void FontBrowserServer::setColors(const QColor &foreground, const QColor &background)
{
  m_selectedFontModel->setColors(foreground, background);
}
