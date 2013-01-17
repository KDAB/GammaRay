/*
  launchpage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "launchpage.h"
#include "ui_launchpage.h"

#include <QCompleter>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSettings>
#include <QStringListModel>

using namespace GammaRay;

LaunchPage::LaunchPage(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::LaunchPage),
    m_argsModel(new QStringListModel(this))
{
  ui->setupUi(this);
  connect(ui->progSelectButton, SIGNAL(clicked()), SLOT(showFileDialog()));
  connect(ui->addArgButton, SIGNAL(clicked()), SLOT(addArgument()));
  connect(ui->removeArgButton, SIGNAL(clicked()), SLOT(removeArgument()));
  connect(ui->progEdit, SIGNAL(textChanged(QString)), SIGNAL(updateButtonState()));

  ui->argsBox->setModel(m_argsModel);

  QCompleter *pathCompleter = new QCompleter(this);
  QFileSystemModel *fsModel = new QFileSystemModel(this);
  fsModel->setRootPath(QDir::rootPath());
  pathCompleter->setModel(fsModel);
  ui->progEdit->setCompleter(pathCompleter);

  QSettings settings;
  ui->progEdit->setText(settings.value(QLatin1String("Launcher/Program")).toString());
  m_argsModel->setStringList(settings.value(QLatin1String("Launcher/Arguments")).toStringList());
  updateArgumentButtons();
}

LaunchPage::~LaunchPage()
{
  delete ui;
}

void LaunchPage::writeSettings()
{
  QSettings settings;
  settings.setValue(QLatin1String("Launcher/Program"), ui->progEdit->text());
  settings.setValue(QLatin1String("Launcher/Arguments"), notEmptyString(m_argsModel->stringList()));
}

QStringList LaunchPage::notEmptyString(const QStringList &list) const
{
  QStringList notEmptyStringList;
  const int numberOfArguments = list.count();
  for (int i = 0; i < numberOfArguments; ++i) {
    if(!list.at(i).trimmed().isEmpty()) {
      notEmptyStringList << list.at(i);
    }
  }
  return notEmptyStringList;
}

QStringList LaunchPage::launchArguments() const
{
  QStringList l;
  l.push_back(ui->progEdit->text());
  l.append(notEmptyString(m_argsModel->stringList()));
  return l;
}

void LaunchPage::showFileDialog()
{
  const QString exeFilePath =
    QFileDialog::getOpenFileName(
      this,
      tr("Executable to Launch"),
      ui->progEdit->text()
#ifdef Q_OS_WIN
      ,tr("Executable (*.exe)")
#endif
    );

  if (exeFilePath.isEmpty()) {
    return;
  }

  ui->progEdit->setText(exeFilePath);
}

void LaunchPage::addArgument()
{
  m_argsModel->insertRows(m_argsModel->rowCount(), 1);
  const QModelIndex newIndex = m_argsModel->index(m_argsModel->rowCount() - 1, 0);
  ui->argsBox->edit(newIndex);
  updateArgumentButtons();
}

void LaunchPage::removeArgument()
{
  // TODO check if there's a selection at all and update button state accordingly
  m_argsModel->removeRows(ui->argsBox->currentIndex().row(), 1);
  updateArgumentButtons();
}

bool LaunchPage::isValid()
{
  if (ui->progEdit->text().isEmpty()) {
    return false;
  }

  const QFileInfo fi(ui->progEdit->text());
  return fi.exists() && fi.isFile() && fi.isExecutable();
}

void LaunchPage::updateArgumentButtons()
{
  ui->removeArgButton->setEnabled(m_argsModel->rowCount() > 0);
}

#include "launchpage.moc"
