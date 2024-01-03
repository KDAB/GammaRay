/*
  metatypebrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_METATYPEBROWSERWIDGET_H
#define GAMMARAY_METATYPEBROWSERWIDGET_H

#include <ui/uistatemanager.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class MetaTypeBrowserWidget;
}

class MetaTypeBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MetaTypeBrowserWidget(QWidget *parent = nullptr);
    ~MetaTypeBrowserWidget() override;

private slots:
    void contextMenu(QPoint pos);

private:
    QScopedPointer<Ui::MetaTypeBrowserWidget> ui;
    UIStateManager m_stateManager;
};
}

#endif // GAMMARAY_METATYPEBROWSERWIDGET_H
