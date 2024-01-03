/*
  methodinvocationdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "methodinvocationdialog.h"
#include "ui_methodinvocationdialog.h"

#include <common/metatypedeclarations.h>

#include <QPushButton>

using namespace GammaRay;

MethodInvocationDialog::MethodInvocationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MethodInvocationDialog)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ui->argumentView->header()->setObjectName("argumentViewHeader");
    ui->argumentView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->argumentView->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->argumentView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Invoke"));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->connectionTypeComboBox->addItem(tr("Auto"), QVariant::fromValue(Qt::AutoConnection));
    ui->connectionTypeComboBox->addItem(tr("Direct"), QVariant::fromValue(Qt::DirectConnection));
    ui->connectionTypeComboBox->addItem(tr("Queued"), QVariant::fromValue(Qt::QueuedConnection));
}

MethodInvocationDialog::~MethodInvocationDialog() = default;

Qt::ConnectionType MethodInvocationDialog::connectionType() const
{
    return ui->connectionTypeComboBox->itemData(ui->connectionTypeComboBox->currentIndex()).value<Qt ::ConnectionType>();
}

void MethodInvocationDialog::setArgumentModel(QAbstractItemModel *model)
{
    ui->argumentView->setModel(model);
}
