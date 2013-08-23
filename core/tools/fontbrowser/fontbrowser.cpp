/*
  fontbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "fontbrowser.h"
#include "ui_fontbrowser.h"

#include "fontmodel.h"

#include <QStandardItemModel>
#include <QFontDatabase>

using namespace GammaRay;

FontBrowser::FontBrowser(ProbeInterface *probe, QObject *parent)
  : QObject(parent)
{
  QFontDatabase database;

  FontModel *selectedFontModel = new FontModel(this);

  QStandardItemModel *model = new QStandardItemModel(this);
  model->setHorizontalHeaderLabels(QStringList() << tr("Fonts") << tr("Smooth sizes"));
  foreach (const QString &family, database.families()) {
    QStandardItem *familyItem = new QStandardItem;
    familyItem->setText(family);

    foreach (const QString &style, database.styles(family)) {
      QStandardItem *styleItem0 = new QStandardItem;
      styleItem0->setText(style);

      QString sizes;
      foreach (int points, database.smoothSizes(family, style)) {
        sizes += QString::number(points) + ' ';
      }

      QStandardItem *styleItem1 = new QStandardItem;
      styleItem1->setText(sizes.trimmed());
      styleItem1->setToolTip(sizes.trimmed());

      familyItem->appendRow(QList<QStandardItem*>() << styleItem0 << styleItem1);
    }

    model->appendRow(familyItem);
  }

  probe->registerModel("com.kdab.GammaRay.FontModel", model);
  probe->registerModel("com.kdab.GammaRay.SelectedFontModel", selectedFontModel);
}

#include "fontbrowser.moc"
