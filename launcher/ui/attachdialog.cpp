/*
  attachdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    margins.setRight(margins.right() + 2);
    margins.setBottom(margins.bottom() + 2);
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
                                                       "Launcher/AttachAccessMode"))
                                        .toInt());

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
    if (ui->stackedWidget->currentWidget() != ui->listViewPage) {
        ui->stackedWidget->setCurrentWidget(ui->listViewPage);
        ui->filter->setFocus();
    }
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
