/*
  methodinvocationdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "methodinvocationdialog.h"
#include "methodargumentmodel.h"
#include <QMessageBox>
#include <QPushButton>

using namespace GammaRay;

Q_DECLARE_METATYPE(Qt::ConnectionType)

MethodInvocationDialog::MethodInvocationDialog(QWidget *parent)
  : QDialog(parent),
    m_argumentModel(new MethodArgumentModel(this))
{
  setAttribute(Qt::WA_DeleteOnClose);

  ui.setupUi(this);

  ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Invoke"));
  connect(ui.buttonBox, SIGNAL(accepted()), SLOT(accept()));
  connect(ui.buttonBox, SIGNAL(rejected()), SLOT(reject()));

  ui.connectionTypeComboBox->addItem(tr("Auto"), Qt::AutoConnection);
  ui.connectionTypeComboBox->addItem(tr("Direct"), Qt::DirectConnection);
  ui.connectionTypeComboBox->addItem(tr("Queued"), Qt::QueuedConnection);

  ui.argumentView->setModel(m_argumentModel);
}

void MethodInvocationDialog::setMethod(QObject *object, const QMetaMethod &method)
{
  m_object = object;
  m_method = method;
  m_argumentModel->setMethod(method);
}

void MethodInvocationDialog::accept()
{
  if (!m_object) {
    QMessageBox::warning(this,
                         tr("Invocation Failed"),
                         tr("Invalid object, probably got deleted in the meantime."));
    QDialog::reject();
    return;
  }

  const Qt::ConnectionType connectionType =
    ui.connectionTypeComboBox->itemData(
      ui.connectionTypeComboBox->currentIndex()).value<Qt::ConnectionType>();
  const QVector<SafeArgument> args = m_argumentModel->arguments();

  const bool result = m_method.invoke(
    m_object.data(), connectionType,
    args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);

  if (!result) {
    QMessageBox::warning(this,
                         tr("Invocation Failed"),
                         tr("Invocation failed, possibly due to mismatching/invalid arguments."));
  }

  QDialog::accept();
}

#include "methodinvocationdialog.moc"
