/*
  selftestpage.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
