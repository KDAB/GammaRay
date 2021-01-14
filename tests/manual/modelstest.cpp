/*
  modelstest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: David Faure <david.faure@kdab.com>

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

#include <QApplication>
#include <QDebug>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QTableView>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QStandardItemModel firstModel(3, 3);
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(
                                                        column));
            firstModel.setItem(row, column, item);
        }
    }
    QTableView firstView;
    firstView.setModel(&firstModel);
    firstView.show();

    QStringListModel stringListModel(QStringList() << "Item1" << "Item2");
    QTableView secondView;
    secondView.setModel(&stringListModel);
    secondView.show();

    return app.exec();
}
