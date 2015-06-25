/*
  mainwindow.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "aboutpluginsdialog.h"
#include "aboutdialog.h"
#include "clienttoolmodel.h"
#include "aboutdata.h"

#include "common/objectbroker.h"
#include "common/modelroles.h"
#include "common/endpoint.h"
#include "common/probecontrollerinterface.h"

#include "kde/krecursivefilterproxymodel.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <private/qguiplatformplugin_p.h>
#else
#include <qpa/qplatformtheme.h>           //krazy:exclude=camelcase
#include <private/qguiapplication_p.h>
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QLabel>
#include <QStyleFactory>

using namespace GammaRay;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
  if (!Endpoint::instance()->isRemoteClient()) {
    // we don't want application styles to propagate to the GammaRay window,
    // so set the platform default one.
    // unfortunately, that's not recursive by default, unless we have a style sheet set
    setStyleSheet(QLatin1String("I_DONT_EXIST {}"));

    QStyle *defaultStyle = 0;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QGuiPlatformPlugin defaultGuiPlatform;
    defaultStyle = QStyleFactory::create(defaultGuiPlatform.styleName());
#else
    foreach (const QString &styleName, QGuiApplicationPrivate::platform_theme->themeHint(QPlatformTheme::StyleNames).toStringList()) {
      if ((defaultStyle = QStyleFactory::create(styleName))) {
        break;
      }
    }
#endif
    if (defaultStyle) {
      // do not set parent of default style
      // this will cause the style being deleted too early through ~QObject()
      // other objects (e.g. the script engine debugger) still might have a
      // reference on the style during destruction
      setStyle(defaultStyle);
    }
  }

  ui->setupUi(this);

  connect(ui->actionRetractProbe, SIGNAL(triggered(bool)), SLOT(detachProbe()));

  connect(QApplication::instance(), SIGNAL(aboutToQuit()), SLOT(close()));
  connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(quitHost()));
  ui->actionQuit->setIcon(QIcon::fromTheme("application-exit"));

  connect(ui->actionPlugins, SIGNAL(triggered(bool)),
          this, SLOT(aboutPlugins()));
  connect(ui->actionAboutQt, SIGNAL(triggered(bool)),
          QApplication::instance(), SLOT(aboutQt()));
  connect(ui->actionAboutGammaRay, SIGNAL(triggered(bool)), SLOT(about()));
  connect(ui->actionAboutKDAB, SIGNAL(triggered(bool)), SLOT(aboutKDAB()));

  setWindowIcon(QIcon(":gammaray/GammaRay-128x128.png"));

  QAbstractItemModel *model = ObjectBroker::model("com.kdab.GammaRay.ToolModel");
  ClientToolModel *toolModel = new ClientToolModel(this);
  toolModel->setData(QModelIndex(), QVariant::fromValue<QWidget*>(this), ToolModelRole::ToolWidgetParent);
  toolModel->setSourceModel(model);
  ui->toolSelector->setModel(toolModel);
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(toolModel);
  ui->toolSelector->setSelectionModel(selectionModel);
  ui->toolSelector->resize(ui->toolSelector->minimumSize());
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(toolSelected()));

  // hide unused tool bar for now
  ui->mainToolBar->setHidden(true);

  setWindowTitle(tr("GammaRay (%1)").arg(Endpoint::instance()->label()));

  selectInitialTool();
  connect(ui->toolSelector->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(selectInitialTool()));
  connect(ui->toolSelector->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(selectInitialTool()));

#ifdef Q_OS_MAC
  ui->groupBox->setFlat(true);
  ui->horizontalLayout->setContentsMargins(0, 0, 0, 0);
#endif

  // get some sane size on startup
  resize(1024, 768);
}

MainWindow::~MainWindow()
{
}

void MainWindow::about()
{
  AboutDialog dialog(this);
  dialog.setWindowTitle(tr("About GammaRay"));
  dialog.setTitle(AboutData::aboutTitle());
  dialog.setText(AboutData::aboutBody());
  dialog.setLogo(":gammaray/GammaRay-128x128.png");
  dialog.setWindowIcon(QPixmap(":gammaray/GammaRay-128x128.png"));
  dialog.exec();
}

void MainWindow::aboutPlugins()
{
  AboutPluginsDialog dlg(this);
  dlg.setFixedSize(800, 600);
  dlg.exec();
}

void MainWindow::aboutKDAB()
{
  AboutDialog dialog(this);
  dialog.setWindowTitle(tr("About KDAB"));
  dialog.setTitle(trUtf8("Klarälvdalens Datakonsult AB (KDAB)"));
  dialog.setText(
    tr("<qt><p>GammaRay is supported and maintained by KDAB</p>"
       "KDAB, the Qt experts, provide consulting and mentoring for developing "
       "Qt applications from scratch and in porting from all popular and legacy "
       "frameworks to Qt. We continue to help develop parts of Qt and are one "
       "of the major contributors to the Qt Project. We can give advanced or "
       "standard trainings anywhere around the globe.</p>"
       "<p>Please visit <a href='http://www.kdab.com'>http://www.kdab.com</a> "
       "to meet the people who write code like this."
       "</p></qt>"));
  dialog.setLogo(":gammaray/kdablogo160.png");
  dialog.setWindowIcon(QPixmap(":gammaray/kdablogo160.png"));
  dialog.exec();
}

void MainWindow::selectInitialTool()
{
  static const QString initialTool("GammaRay::ObjectInspector");

  QAbstractItemModel *model = ui->toolSelector->model();
  QModelIndexList matches =
    model->match(model->index(0, 0), ToolModelRole::ToolId, initialTool);
  if (matches.isEmpty()) {
    return;
  }

  disconnect(ui->toolSelector->model(), 0, this, SLOT(selectInitialTool()));
  ui->toolSelector->setCurrentIndex(matches.first());
  toolSelected();
}

void MainWindow::toolSelected()
{
  ui->actionsMenu->clear();
  QModelIndexList list = ui->toolSelector->selectionModel()->selectedRows();
  int row = -1;
  if (list.count()) {
    row = list[0].row();
  }
  if (row == -1) {
    return;
  }

  const QModelIndex mi = ui->toolSelector->model()->index(row, 0);
  QWidget *toolWidget = mi.data(ToolModelRole::ToolWidget).value<QWidget*>();
  if (!toolWidget) {
    toolWidget = createErrorPage(mi);
    ui->toolSelector->model()->setData(mi, QVariant::fromValue(toolWidget), ToolModelRole::ToolWidget);
  }

  Q_ASSERT(toolWidget);
  if (ui->toolStack->indexOf(toolWidget) < 0) { // newly created
    if (toolWidget->layout()) {
#ifndef Q_OS_MAC
      toolWidget->layout()->setContentsMargins(11, 0, 0, 0);
#else
      QMargins margins = toolWidget->layout()->contentsMargins();
      margins.setLeft(0);
      toolWidget->layout()->setContentsMargins(margins);
#endif
    }
    ui->toolStack->addWidget(toolWidget);
  }

  ui->toolStack->setCurrentIndex(ui->toolStack->indexOf(toolWidget));

  foreach (QAction *action, toolWidget->actions()) {
    ui->actionsMenu->addAction(action);
  }
  ui->actionsMenu->setEnabled(!ui->actionsMenu->isEmpty());
}

QWidget *MainWindow::createErrorPage(const QModelIndex &index)
{
  QLabel *page = new QLabel(this);
  page->setAlignment(Qt::AlignCenter);
  // TODO show the actual plugin error message as well as any other useful information (eg. file name) we have, once the tool model has those
  page->setText(tr("Tool %1 failed to load.").arg(index.data(ToolModelRole::ToolId).toString()));
  return page;
}

void MainWindow::quitHost()
{
  emit targetQuitRequested();
  ObjectBroker::object<ProbeControllerInterface*>()->quitHost();
}

void MainWindow::detachProbe()
{
  emit targetQuitRequested();
  ObjectBroker::object<ProbeControllerInterface*>()->detachProbe();
}
