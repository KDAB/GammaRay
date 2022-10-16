/*
  aboutpluginsdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "aboutpluginsdialog.h"
#include <common/pluginmanager.h>
#include <common/objectbroker.h>

#include <QAbstractTableModel>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>

using namespace GammaRay;

AboutPluginsDialog::AboutPluginsDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    QLayout *layout = nullptr;
    auto *vbox = new QVBoxLayout(this);

    {
        auto *toolView = new QTableView(this);
        toolView->setShowGrid(false);
        toolView->setSelectionBehavior(QAbstractItemView::SelectRows);
        toolView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        toolView->verticalHeader()->hide();
        toolView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ToolPluginModel")));

        QGroupBox *toolBox = new QGroupBox(tr("Loaded Plugins"), this);
        layout = new QHBoxLayout(toolBox);
        layout->addWidget(toolView);
        vbox->addWidget(toolBox);
    }

    {
        auto *errorView = new QTableView(this);
        errorView->setShowGrid(false);
        errorView->setSelectionBehavior(QAbstractItemView::SelectRows);
        errorView->setModel(ObjectBroker::model(QStringLiteral(
            "com.kdab.GammaRay.ToolPluginErrorModel")));
        errorView->verticalHeader()->hide();
        errorView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QGroupBox *errorBox = new QGroupBox(tr("Failed Plugins"), this);
        layout = new QHBoxLayout(errorBox);
        layout->addWidget(errorView);
        vbox->addWidget(errorBox);
    }

    setWindowTitle(tr("GammaRay: Plugin Info"));
}
