/*
  mainwindow.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "config-gammaray-version.h"
#include "mainwindow.h"
#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "objecttypefilterproxymodel.h"
#include "toolmodel.h"
#include "toolfactory.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QCoreApplication>
#include <qdebug.h>
#include <QtGui/QStringListModel>
#include <QtCore/qtextcodec.h>
#include <QtGui/QMessageBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QTreeView>
#include <QHBoxLayout>

using namespace GammaRay;

static const char progName[] = PROGRAM_NAME;
static const char progVersion[] = GAMMARAY_VERSION_STRING;
static const char progDesc[] = "The Qt application inspection and manipulation tool";
static const char progURL[] = "http://www.kdab.com/gammaray";

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
  ui.setupUi(this);

  connect(ui.actionRetractProbe, SIGNAL(triggered(bool)), SLOT(close()));

  connect(QApplication::instance(), SIGNAL(aboutToQuit()), SLOT(close()));
  connect(ui.actionQuit, SIGNAL(triggered(bool)),
          QApplication::instance(), SLOT(quit()));
  ui.actionQuit->setIcon(QIcon::fromTheme("application-exit"));

  connect(ui.actionAboutQt, SIGNAL(triggered(bool)),
          QApplication::instance(), SLOT(aboutQt()));
  connect(ui.actionAboutGammaRay, SIGNAL(triggered(bool)), SLOT(about()));
  connect(ui.actionAboutKDAB, SIGNAL(triggered(bool)), SLOT(aboutKDAB()));

  setWindowIcon(QIcon(":gammaray/GammaRay-128x128.png"));

  ToolModel *model = Probe::instance()->toolModel();
  QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setDynamicSortFilter(true);
  proxyModel->setSourceModel(model);
  proxyModel->sort(0);
  ui.toolSelector->setModel(proxyModel);
  ui.toolSelector->resize(ui.toolSelector->minimumSize());
  connect(ui.toolSelector->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(toolSelected()));

  // hide unused tool bar for now
  ui.mainToolBar->setHidden(true);

  setWindowTitle(tr("%1 (%2)").arg(progName).arg(qApp->applicationName()));

  selectInitialTool();

  // get some sane size on startup
  resize(1024, 768);
}

void MainWindow::about()
{
  QMessageBox mb(this);
  mb.setWindowTitle(tr("About %1").arg(progName));
  mb.setText(tr("<b>%1 %2</b><p>%3").arg(progName).arg(progVersion).arg(progDesc));
  mb.setInformativeText(
    trUtf8("<qt><p>Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, "
       "a KDAB Group company, <a href='mailto:info@kdab.com'>info@kdab.com</a></p>"
       "<p><u>Authors:</u><br>"
       "Volker Krause &lt;volker.krause@kdab.com&gt; (Head Engineer)<br>"
       "Allen Winter &lt;allen.winter@kdab.com&gt;<br>"
       "Andreas Holzammer &lt;andreas.holzammer@kdab.com&gt;<br>"
       "David Faure &lt;david.faure@kdab.com&gt;<br>"
       "Kevin Funk &lt;kevin.funk@kdab.com&gt;<br>"
       "Milian Wolff &lt;milian.wolff@kdab.com&gt;<br>"
       "Patrick Spendrin &lt;patrick.spendrin@kdab.com&gt;<br>"
       "Stephen Kelly &lt;stephen.kelly@kdab.com&gt;<br>"
       "Till Adam &lt;till@kdab.com&gt;<br>"
       "Tobias Koenig &lt;tobias.koenig@kdab.com&gt;<br></p></qt>"));
  mb.setIconPixmap(QPixmap(":gammaray/GammaRay-128x128.png"));
  mb.addButton(QMessageBox::Close);
  mb.exec();
}

void MainWindow::aboutKDAB()
{
  QMessageBox mb(this);
  mb.setWindowTitle(tr("About KDAB"));
  mb.setText(trUtf8("Klarälvdalens Datakonsult AB (KDAB)"));
  mb.setInformativeText(
    tr("<qt><p>%1 is supported and maintained by KDAB</p>"
       "KDAB, the Qt experts, provide consulting and mentoring for developing "
       "Qt applications from scratch and in porting from all popular and legacy "
       "frameworks to Qt. Our software products increase Qt productivity and our "
       "Qt trainers have trained 50% of commercial Qt developers globally.</p>"
       "<p>Please visit <a href='http://www.kdab.com'>http://www.kdab.com</a> "
       "to meet the people who write code like this. "
       "We also offer Qt training courses."
       "</p></qt>").arg(progName));
  mb.setIconPixmap(QPixmap(":gammaray/kdablogo160.png"));
  mb.addButton(QMessageBox::Close);
  mb.exec();
}

void MainWindow::selectInitialTool()
{
  static const QString initialTool("GammaRay::ObjectInspector");

  QAbstractItemModel *model = ui.toolSelector->model();
  QModelIndexList matches =
    model->match(model->index(0, 0), ToolModel::ToolIdRole, initialTool);
  if (matches.isEmpty()) {
    return;
  }

  ui.toolSelector->setCurrentIndex(matches.first());
  toolSelected();
}

void MainWindow::toolSelected()
{
  const int row = ui.toolSelector->currentIndex().row();
  if (row == -1) {
    return;
  }

  const QModelIndex mi = ui.toolSelector->model()->index(row, 0);
  QWidget *toolWidget = mi.data(ToolModel::ToolWidgetRole).value<QWidget*>();
  if (!toolWidget) {
    ToolFactory *toolIface = mi.data(ToolModel::ToolFactoryRole).value<ToolFactory*>();
    Q_ASSERT(toolIface);
//     qDebug() << Q_FUNC_INFO << "creating new probe: "
//              << toolIface->name() << toolIface->supportedTypes();
    toolWidget = toolIface->createWidget(Probe::instance(), this);
    toolWidget->layout()->setContentsMargins(11, 0, 0, 0);
    ui.toolStack->addWidget(toolWidget);
    ui.toolSelector->model()->setData(mi, QVariant::fromValue(toolWidget));
  }
  ui.toolStack->setCurrentIndex(ui.toolStack->indexOf(toolWidget));
}

#include "mainwindow.moc"
