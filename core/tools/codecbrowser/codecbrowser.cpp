/*
  codecbrowser.cpp

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

#include "codecbrowser.h"
#include "ui_codecbrowser.h"

#include "codecmodel.h"

using namespace GammaRay;

CodecBrowser::CodecBrowser(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::CodecBrowser)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  ui->codecList->setRootIsDecorated(false);
  ui->codecList->setModel(new AllCodecsModel(this));
  ui->codecList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_selectedCodecsModel = new SelectedCodecsModel(this);
  ui->selectedCodecs->setRootIsDecorated(false);
  ui->selectedCodecs->setModel(m_selectedCodecsModel);

  ui->codecList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui->selectedCodecs->header()->setResizeMode(0, QHeaderView::ResizeToContents);

  connect(ui->codecList->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(updateCodecs(QItemSelection,QItemSelection)));
  connect(ui->codecText, SIGNAL(textChanged(QString)),
          m_selectedCodecsModel, SLOT(updateText(QString)));
}

void CodecBrowser::updateCodecs(const QItemSelection &selected,
                                const QItemSelection &deselected)
{
  Q_UNUSED(selected);
  Q_UNUSED(deselected);

  QStringList currentCodecNames;
  foreach (const QModelIndex &index, ui->codecList->selectionModel()->selectedRows()) {
    const QString codecName = index.data().toString();
    currentCodecNames.append(codecName);
  }

  m_selectedCodecsModel->setCodecs(currentCodecNames);
}

#include "codecbrowser.moc"
