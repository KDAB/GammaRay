/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "attachdialog.h"
#include "processfiltermodel.h"
#include "processmodel.h"
#include "probeabimodel.h"
#include "processlist.h"

#include "ui_attachdialog.h"

#include <launcher/core/launchoptions.h>

#include <ui/searchlinecontroller.h>

#include <QPushButton>
#include <QStandardItemModel>
#include <QListView>
#include <QTimer>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QStackedWidget>
#include <QStringListModel>
#include <QSettings>

using namespace GammaRay;

AttachDialog::AttachDialog(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , ui(new Ui::AttachDialog)
    , m_abiModel(new ProbeABIModel(this))
{
    ui->setupUi(this);
#if defined(Q_OS_MAC)
    QMargins margins = ui->formLayout->contentsMargins();
    margins.setRight(margins.right() +2);
    margins.setBottom(margins.bottom() +2);
    ui->formLayout->setContentsMargins(margins);
#endif

    m_model = new ProcessModel(this);

    m_proxyModel = new ProcessFilterModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setDynamicSortFilter(true);

    ui->view->setModel(m_proxyModel);
    // hide state
    ui->view->hideColumn(ProcessModel::StateColumn);
    ui->view->sortByColumn(ProcessModel::NameColumn, Qt::AscendingOrder);
    ui->view->setSortingEnabled(true);

    ui->view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->view->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->view->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &AttachDialog::updateButtonState);
    connect(ui->view->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &AttachDialog::selectABI);

    connect(ui->view, &QAbstractItemView::activated, this, &AttachDialog::activate);

    new SearchLineController(ui->filter, m_proxyModel);

    ui->probeBox->setModel(m_abiModel);

    QSettings settings;
    ui->accessMode->setCurrentIndex(settings.value(QStringLiteral(
                                                      "Launcher/AttachAccessMode")).toInt());

    setWindowTitle(tr("GammaRay - Attach to Process"));
    setWindowIcon(QIcon(QStringLiteral(":/gammaray/GammaRay-128x128.png")));

    ui->stackedWidget->setCurrentWidget(ui->loadingLabel);
    updateProcesses();
}

AttachDialog::~AttachDialog() = default;

bool AttachDialog::isValid() const
{
    return ui->view->currentIndex().isValid();
}

void AttachDialog::writeSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("Launcher/AttachAccessMode"), ui->accessMode->currentIndex());
}

void AttachDialog::setSettingsVisible(bool visible)
{
    ui->settingsWidget->setVisible(visible);
}

LaunchOptions AttachDialog::launchOptions() const
{
    LaunchOptions opt;
    opt.setPid(pid());
    opt.setProbeABI(ui->probeBox->itemData(ui->probeBox->currentIndex()).value<ProbeABI>());

    switch (ui->accessMode->currentIndex()) {
    case 0: // local, out-of-process
        opt.setProbeSetting(QStringLiteral("RemoteAccessEnabled"), true);
        opt.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        opt.setUiMode(LaunchOptions::OutOfProcessUi);
        break;
    case 1: // remote, out-of-process
        opt.setProbeSetting(QStringLiteral("RemoteAccessEnabled"), true);
        opt.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_ANY_TCP_URL);
        opt.setUiMode(LaunchOptions::OutOfProcessUi);
        break;
    case 2: // in-process
        opt.setProbeSetting(QStringLiteral("RemoteAccessEnabled"), false);
        opt.setUiMode(LaunchOptions::InProcessUi);
        break;
    }

    return opt;
}

int AttachDialog::pid() const
{
    return ui->view->currentIndex().data(ProcessModel::PIDRole).toInt();
}

QString GammaRay::AttachDialog::absoluteExecutablePath() const
{
    return ui->view->currentIndex().data(ProcessModel::NameRole).toString();
}

void AttachDialog::updateProcesses()
{
    auto *watcher = new QFutureWatcher<ProcDataList>(this);
    connect(watcher, &QFutureWatcherBase::finished,
            this, &AttachDialog::updateProcessesFinished);
    watcher->setFuture(QtConcurrent::run(processList, m_model->processes()));
}

void AttachDialog::updateProcessesFinished()
{
    QFutureWatcher<ProcDataList> *watcher = dynamic_cast<QFutureWatcher<ProcDataList> *>(sender());
    Q_ASSERT(watcher);
    ui->stackedWidget->setCurrentWidget(ui->listViewPage);
    const int oldPid = pid();
    m_model->mergeProcesses(watcher->result());
    if (oldPid != pid())
        ui->view->setCurrentIndex(QModelIndex());
    watcher->deleteLater();

    QTimer::singleShot(1000, this, &AttachDialog::updateProcesses);
}

void AttachDialog::selectABI(const QModelIndex &processIndex)
{
    if (!processIndex.isValid())
        return;

    const ProbeABI abi = processIndex.data(ProcessModel::ABIRole).value<ProbeABI>();
    const int abiIndex = m_abiModel->indexOfBestMatchingABI(abi);
    if (abiIndex >= 0)
        ui->probeBox->setCurrentIndex(abiIndex);
}
