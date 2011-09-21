/*
  attachdialog.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "attachdialog.h"

#include "processfiltermodel.h"
#include "processmodel.h"

#include <QPushButton>
#include <QStandardItemModel>
#include <QListView>

using namespace Endoscope;

AttachDialog::AttachDialog(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent, f)
{
  ui.setupUi(this);
  ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));

  m_model = new ProcessModel(this);
  m_model->addProcesses(processList());

  m_proxyModel = new ProcessFilterModel(this);
  m_proxyModel->setSourceModel(m_model);

  ui.view->setModel(m_proxyModel);
  // hide state
  ui.view->hideColumn(ProcessModel::StateColumn);
  ui.view->sortByColumn(ProcessModel::NameColumn, Qt::AscendingOrder);
  ui.view->setSortingEnabled(true);

  ui.view->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui.view->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui.view->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(ui.view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
          this, SLOT(selectionChanged()));

  connect(ui.view, SIGNAL(activated(QModelIndex)),
          ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

  ui.filter->setProxy(m_proxyModel);

  setWindowTitle(tr("Endoscope - Attach to Process"));
  setWindowIcon(QIcon(":endoscope/endoscope128.png"));

  selectionChanged();
}

void AttachDialog::selectionChanged()
{
  ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui.view->currentIndex().isValid());
}

QString AttachDialog::pid() const
{
  return ui.view->currentIndex().data(ProcessModel::PIDRole).toString();
}

#include "attachdialog.moc"
