/*
  positioningwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_POSITIONINGWIDGET_H
#define GAMMARAY_POSITIONINGWIDGET_H

#include <ui/tooluifactory.h>

#include <QScopedPointer>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QGeoPositionInfo;
class QNmeaPositionInfoSource;
QT_END_NAMESPACE

namespace GammaRay {

namespace Ui {
class PositioningWidget;
}

class MapController;
class PositioningInterface;

class PositioningWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PositioningWidget(QWidget *parent = nullptr);
    ~PositioningWidget() override;

private slots:
    void updatePosition();
    void replayPosition();
    void loadNmeaFile();
    void nmeaError();
    void updateWidgetState();

private:
    void setUiToPosition(const QGeoPositionInfo &pos);

    QScopedPointer<Ui::PositioningWidget> ui;
    PositioningInterface *m_interface;
    MapController *m_mapController;
    QNmeaPositionInfoSource *m_replaySource;
    bool m_updateLock;
};

class PositioningUiFactory : public QObject, public StandardToolUiFactory<PositioningWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_positioning.json")
public:
    void initUi() override;
};

}

#endif // GAMMARAY_POSITIONINGWIDGET_H
