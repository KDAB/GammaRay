/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and
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
#include <QTimer>
#include <QFutureWatcher>
#include <QtConcurrentRun>

using namespace GammaRay;

AttachDialog::AttachDialog(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent, f)
{
  ui.setupUi(this);
  ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));

  m_model = new ProcessModel(this);

  m_proxyModel = new ProcessFilterModel(this);
  m_proxyModel->setSourceModel(m_model);
  m_proxyModel->setDynamicSortFilter(true);

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

  setWindowTitle(tr("GammaRay - Attach to Process"));
  setWindowIcon(QIcon(":gammaray/GammaRay-128x128.png"));

  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateProcesses()));
  m_timer->start(1000);

  // set process list syncronously the first time
  m_model->setProcesses(processList());
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

void AttachDialog::updateProcesses()
{
  QFutureWatcher<ProcDataList>* watcher = new QFutureWatcher<ProcDataList>(this);
  connect(watcher, SIGNAL(finished()),
          this, SLOT(updateProcessesFinished()));
  watcher->setFuture(QtConcurrent::run(processList));
}

void AttachDialog::updateProcessesFinished()
{
  QFutureWatcher<ProcDataList>* watcher = dynamic_cast<QFutureWatcher<ProcDataList>*>(sender());
  Q_ASSERT(watcher);
  const QString oldPid = pid();
  m_model->mergeProcesses(watcher->result());
  if (oldPid != pid()) {
    ui.view->setCurrentIndex(QModelIndex());
  }
  watcher->deleteLater();
}

#include "attachdialog.moc"
