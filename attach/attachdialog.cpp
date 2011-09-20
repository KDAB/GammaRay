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

#include <QPushButton>
#include <QStandardItemModel>
#include <QListView>
#include <QDesktopServices>
#include <QUrl>

using namespace Endoscope;

AttachDialog::AttachDialog(QWidget *parent, Qt::WindowFlags f)
: QDialog(parent, f)
{
  ui.setupUi(this);
  ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));

  model = new ProcessListFilterModel(this);
  model->populate(processList(), QString::number(qApp->applicationPid()));
  ui.view->setModel(model);

  ui.view->setEditTriggers(QAbstractItemView::NoEditTriggers);

  ui.view->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui.view->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(ui.view->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
          this, SLOT(selectionChanged()));

  connect(ui.view, SIGNAL(activated(QModelIndex)),
          ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

  ui.filter->setProxy(model);

  setWindowTitle(tr("Endoscope - Attach to Process"));
  setWindowIcon(QIcon(":endoscope/endoscope128.png"));
  QString promoLabelText = QLatin1String(
      "<html>"
        "<a href='http://www.kdab.com'>"
          "<img src=':endoscope/kdabproducts.png' />"
        "</a>"
      "</html>");
  ui.promoLabel->setToolTip(tr("Visit KDAB Website"));
  ui.promoLabel->setText(promoLabelText);
  connect(ui.promoLabel, SIGNAL(linkActivated(QString)),
          this, SLOT(linkActivated(QString)));

  selectionChanged();
}

void AttachDialog::selectionChanged()
{
  ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui.view->currentIndex().isValid());
}

QString AttachDialog::pid()
{
  return model->processIdAt(ui.view->currentIndex());
}

void AttachDialog::linkActivated(const QString &link)
{
  QDesktopServices::openUrl(QUrl(link));
}

#include "attachdialog.moc"
