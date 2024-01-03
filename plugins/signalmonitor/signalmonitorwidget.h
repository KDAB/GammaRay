/*
  signalmonitorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALMONITORWIDGET_H
#define GAMMARAY_SIGNALMONITORWIDGET_H

#include <ui/uistatemanager.h>
#include "signalhistoryview.h"
#include "ui/favoritesitemview.h"
#include "ui/tooluifactory.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QSlider;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class SignalMonitorWidget;
}

class SignalHistoryFavoritesView : public FavoritesItemView<GammaRay::SignalHistoryView>
{
    Q_OBJECT
    using Super = FavoritesItemView<GammaRay::SignalHistoryView>;

public:
    SignalHistoryFavoritesView(QWidget *parent = nullptr);
};

class SignalMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalMonitorWidget(QWidget *parent = nullptr);
    ~SignalMonitorWidget() override;

    QSlider *zoomSlider();

private slots:
    void intervalScaleValueChanged(int value);
    void adjustEventScrollBarSize();
    void pauseAndResume(bool pause);
    void eventDelegateIsActiveChanged(bool active);
    void contextMenu(QPoint pos);
    void selectionChanged(const QItemSelection &selection);

private:
    static const QString ITEM_TYPE_NAME_OBJECT;
    QScopedPointer<Ui::SignalMonitorWidget> ui;
    UIStateManager m_stateManager;
};

class SignalMonitorUiFactory : public QObject, public StandardToolUiFactory<SignalMonitorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_signalmonitor.json")
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITORWIDGET_H
