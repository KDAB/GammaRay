/*
  widgetremoteview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WIDGETREMOTEVIEW_H
#define GAMMARAY_WIDGETREMOTEVIEW_H

#include <ui/remoteviewwidget.h>

namespace GammaRay {

class WidgetRemoteView : public RemoteViewWidget
{
    Q_OBJECT
public:
    explicit WidgetRemoteView(QWidget *parent = nullptr);
    ~WidgetRemoteView() override;

public slots:
    void setTabFocusOverlayEnabled(bool enabled);

protected:
    void drawDecoration(QPainter *p) override;

private:
    bool m_tabFocusEnabled;
};
}

#endif // GAMMARAY_WIDGETREMOTEVIEW_H
