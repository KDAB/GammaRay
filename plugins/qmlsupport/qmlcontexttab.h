/*
  qmlcontexttab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMLCONTEXTTAB_H
#define GAMMARAY_QMLCONTEXTTAB_H

#include <ui/propertywidget.h>

#include <QScopedPointer>

namespace GammaRay {
namespace Ui {
class QmlContextTab;
}

class QmlContextTab : public QWidget
{
    Q_OBJECT
public:
    explicit QmlContextTab(PropertyWidget *parent = nullptr);
    ~QmlContextTab() override;

private:
    void contextContextMenu(QPoint pos);
    void propertiesContextMenu(QPoint pos);
    QScopedPointer<Ui::QmlContextTab> ui;
};
}

#endif // GAMMARAY_QMLCONTEXTTAB_H
