/*
  selftestpage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "selftestpage.h"
#include "ui_selftestpage.h"

#include <launcher/core/probefinder.h>
#include <launcher/core/selftest.h>

#include <QStandardItemModel>

using namespace GammaRay;

SelfTestPage::SelfTestPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SelfTestPage)
    , m_resultModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->resultView->setModel(m_resultModel);
    run();
}

SelfTestPage::~SelfTestPage()
{
    delete ui;
}

void SelfTestPage::run()
{
    m_resultModel->clear();
    SelfTest selfTest;
    connect(&selfTest, &SelfTest::information, this, &SelfTestPage::information);
    connect(&selfTest, &SelfTest::error, this, &SelfTestPage::error);
    selfTest.checkEverything();
}

void SelfTestPage::error(const QString &msg)
{
    auto *item = new QStandardItem;
    item->setEditable(false);
    item->setText(msg);
    item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
    m_resultModel->appendRow(item);
}

void SelfTestPage::information(const QString &msg)
{
    auto *item = new QStandardItem;
    item->setEditable(false);
    item->setText(msg);
    item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    m_resultModel->appendRow(item);
}
