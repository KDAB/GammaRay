/*
  networkreplywidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_NETWORKREPLYWIDGET_H
#define GAMMARAY_NETWORKREPLYWIDGET_H

#include <QWidget>

#include <memory>

namespace GammaRay {

namespace Ui {
class NetworkReplyWidget;
}

class NetworkReplyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkReplyWidget(QWidget *parent = nullptr);
    ~NetworkReplyWidget();

private:
    void contextMenu(QPoint pos);

    std::unique_ptr<Ui::NetworkReplyWidget> ui;
};

}

#endif // GAMMARAY_NETWORKREPLYWIDGET_H
