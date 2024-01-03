/*
  qmltypetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLTYPETAB_H
#define GAMMARAY_QMLTYPETAB_H

#include <QScopedPointer>
#include <QWidget>

namespace GammaRay {
class PropertyWidget;

namespace Ui {
class QmlTypeTab;
}

class QmlTypeTab : public QWidget
{
    Q_OBJECT
public:
    explicit QmlTypeTab(PropertyWidget *parent = nullptr);
    ~QmlTypeTab() override;

private:
    void contextMenu(QPoint pos);

    QScopedPointer<Ui::QmlTypeTab> ui;
};
}

#endif // GAMMARAY_QMLTYPETAB_H
