/*
  modelstest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: David Faure <david.faure@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            firstModel.setItem(row, column, item);
        }
    }
    QTableView firstView;
    firstView.setModel(&firstModel);
    firstView.show();

    QStringListModel stringListModel(QStringList() << "Item1"
                                                   << "Item2");
    QTableView secondView;
    secondView.setModel(&stringListModel);
    secondView.show();

    return app.exec();
}
