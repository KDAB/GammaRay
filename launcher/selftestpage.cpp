/*
  selftestpage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "selftestpage.h"
#include "ui_selftestpage.h"
#include "probefinder.h"

#include <QStandardItemModel>
#include <qfileinfo.h>
#include <injector/injectorfactory.h>

using namespace GammaRay;

SelfTestPage::SelfTestPage(QWidget *parent)
  : QWidget(parent), ui(new Ui::SelfTestPage), m_resultModel(new QStandardItemModel(this))
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
  testProbe();
  testAvailableInjectors();
  testInjectors();
}

void SelfTestPage::testProbe()
{
  const QString probePath = ProbeFinder::findProbe(QLatin1String("gammaray_probe"));
  if (probePath.isEmpty()) {
    error(tr("No probe found - GammaRay not functional."));
    return;
  }

  QFileInfo fi(probePath);
  if (!fi.exists() || !fi.isFile() || !fi.isReadable()) {
    error(tr("Probe at %1 is invalid - GammaRay not functional."));
    return;
  }

  information(tr("Found valid probe at %1.").arg(probePath));
}

void SelfTestPage::testAvailableInjectors()
{
  const QStringList injectors = InjectorFactory::availableInjectors();
  if (injectors.isEmpty()) {
    error(tr("No injectors available - GammaRay not functional."));
    return;
  }

  information(tr("The following injectors are available: %1").
              arg(injectors.join(QLatin1String(", "))));
}

void SelfTestPage::testInjectors()
{
  foreach (const QString &injectorType, InjectorFactory::availableInjectors()) {
    AbstractInjector::Ptr injector = InjectorFactory::createInjector(injectorType);
    if (!injector) {
      error(tr("Unable to create instance of injector %1.").arg(injectorType));
      continue;
    }
    if (injector->selfTest()) {
      information(tr("Injector %1 successfully passed its self-test.").
                  arg(injectorType));
    } else {
      error(tr("Injector %1 failed to pass its self-test: %2.").
            arg(injectorType, injector->errorString()));
    }
  }
}

void SelfTestPage::error(const QString &msg)
{
  QStandardItem *item = new QStandardItem;
  item->setText(msg);
  item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
  m_resultModel->appendRow(item);
}

void SelfTestPage::information(const QString &msg)
{
  QStandardItem *item = new QStandardItem;
  item->setText(msg);
  item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
  m_resultModel->appendRow(item);
}

#include "selftestpage.moc"
