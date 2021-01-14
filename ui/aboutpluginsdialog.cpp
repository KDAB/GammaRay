/*
  aboutpluginsdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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
