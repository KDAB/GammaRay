/*
  aboutdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    return { 960, 730 };
}
