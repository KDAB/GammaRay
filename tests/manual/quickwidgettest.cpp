/*
  quickwidgettest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include <QQuickWidget>
#include <QLabel>
#include <QVBoxLayout>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget top;
    top.setWindowTitle(QStringLiteral("Nested Quick Widget"));
    auto topLayout = new QVBoxLayout;
    top.setLayout(topLayout);

    auto topLabel = new QLabel(QStringLiteral("Label outside QQ2 widget"));
    topLayout->addWidget(topLabel);

    auto qqw = new QQuickWidget;
    qqw->setSource(QUrl(QStringLiteral("qrc:/reparenttest.qml")));
    topLayout->addWidget(qqw);

    auto subLayout = new QVBoxLayout;
    qqw->setLayout(subLayout);
    auto subLabel = new QLabel(QStringLiteral("Label over QQ2 widget"));
    subLayout->addWidget(subLabel);

    top.show();

    QQuickWidget top2;
    top2.setWindowTitle(QStringLiteral("Top Quick Widget"));
    top2.setSource(QUrl(QStringLiteral("qrc:/reparenttest.qml")));

    subLayout = new QVBoxLayout;
    top2.setLayout(subLayout);
    subLabel = new QLabel(QStringLiteral("Label over QQ2 top level widget"));
    subLayout->addWidget(subLabel);

    top2.show();

    return app.exec();
}
