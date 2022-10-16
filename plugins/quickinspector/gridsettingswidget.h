/*
  gridsettingswidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_GRIDSETTINGSWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_GRIDSETTINGSWIDGET_H

#include <QWidget>

namespace GammaRay {

struct QuickDecorationsSettings;
namespace Ui {
class GridSettingsWidget;
}

class GridSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridSettingsWidget(QWidget *parent = nullptr);
    ~GridSettingsWidget() override;

    void setOverlaySettings(const QuickDecorationsSettings &settings);

signals:
    void enabledChanged(bool enabled);
    void offsetChanged(const QPoint &value);
    void cellSizeChanged(const QSize &value);

private:
    Ui::GridSettingsWidget *ui;

private slots:
    void offsetUserChanged();
    void cellSizeUserChanged();
};

}

#endif // GAMMARAY_QUICKINSPECTOR_GRIDSETTINGSWIDGET_H
