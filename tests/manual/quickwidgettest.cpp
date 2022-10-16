/*
  quickwidgettest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
