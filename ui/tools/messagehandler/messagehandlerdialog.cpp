/*
  messagehandlerdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "messagehandlerdialog.h"
#include "ui_messagehandlerdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QTime>


using namespace GammaRay;

MessageHandlerDialog::MessageHandlerDialog(QWidget* parent): QDialog(parent),
  ui(new Ui::MessageHandlerDialog)
{
  ui->setupUi(this);

  QPushButton *copyBacktraceButton = new QPushButton(tr("Copy Backtrace"));
  ui->buttons->addButton(copyBacktraceButton, QDialogButtonBox::ActionRole);
  connect(copyBacktraceButton, SIGNAL(clicked()), this, SLOT(copyBacktraceToClipboard()));

  QIcon icon = style()->standardIcon(QStyle::SP_MessageBoxCritical, 0, this);
  int iconSize = style()->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, this);
  ui->iconLabel->setPixmap(icon.pixmap(iconSize, iconSize));
  ui->iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

MessageHandlerDialog::~MessageHandlerDialog()
{
}

void MessageHandlerDialog::setTitleData(const QString &app, const QTime &time)
{
    setWindowTitle(tr("QFatal in %1 at %2").arg(app).arg(time.toString()));
}

void MessageHandlerDialog::setMessage(const QString &message)
{
    ui->errorLabel->setText(message);
}

void MessageHandlerDialog::setBacktrace(const QStringList &backtrace)
{
    foreach (const QString &frame, backtrace) {
      ui->backtraceWidget->addItem(frame);
    }
    m_backtrace = backtrace;
}

void MessageHandlerDialog::copyBacktraceToClipboard()
{
    QApplication::clipboard()->setText(m_backtrace.join(QLatin1String("\n")));
}
