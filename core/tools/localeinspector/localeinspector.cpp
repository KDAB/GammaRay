/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "localeinspector.h"
#include "ui_localeinspector.h"

#include "localemodel.h"
#include "localeaccessormodel.h"

#include <QTableView>

using namespace GammaRay;

LocaleInspector::LocaleInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::LocaleInspector)
{
  Q_UNUSED(probe);

  LocaleModel *model = new LocaleModel(this);
  LocaleAccessorModel *accessorModel = new LocaleAccessorModel(this);

  ui->setupUi(this);

  ui->localeTable->setModel(model);
  ui->accessorTable->setModel(accessorModel);

  ui->accessorTable->resizeColumnsToContents();
  ui->localeTable->resizeColumnsToContents();
  connect(model, SIGNAL(modelReset()), ui->localeTable, SLOT(resizeColumnsToContents()));
}

#include "localeinspector.moc"
