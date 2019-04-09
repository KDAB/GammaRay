/*
  aboutdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "aboutdialog.h"
#include "aboutwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

using namespace GammaRay;

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new AboutWidget)
{
    auto button = new QDialogButtonBox(this);
    button->setStandardButtons(QDialogButtonBox::Close);

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(ui);
    vl->addWidget(button);

    connect(button, &QDialogButtonBox::rejected, this, &QWidget::close);
}

AboutDialog::~AboutDialog() = default;

void AboutDialog::setLogo(const QString &iconFileName)
{
    ui->setLogo(iconFileName);
}

void AboutDialog::setThemeLogo(const QString &fileName)
{
    ui->setThemeLogo(fileName);
}

void AboutDialog::setTitle(const QString &title)
{
    ui->setTitle(title);
}

void AboutDialog::setHeader(const QString &header)
{
    ui->setHeader(header);
}

void AboutDialog::setAuthors(const QString &authors)
{
    ui->setAuthors(authors);
}

void AboutDialog::setFooter(const QString &footer)
{
    ui->setFooter(footer);
}

void AboutDialog::setText(const QString &text)
{
    ui->setText(text);
}

QSize AboutDialog::sizeHint() const
{
    return {960, 730};
}
