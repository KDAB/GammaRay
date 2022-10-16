/*
  logview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOGVIEW_H
#define GAMMARAY_LOGVIEW_H

#include <QScrollArea>
#include <QTabWidget>

namespace GammaRay {

class Messages;
class Timeline;

class LogView : public QTabWidget
{
    Q_OBJECT
public:
    explicit LogView(QWidget *p);

    QSize sizeHint() const override;
    void logMessage(quint64 pid, qint64 time, const QByteArray &msg);
    void setLoggingClient(quint64 pid);
    void reset();

private:
    Messages *m_messages;
    Timeline *m_timeline;
};

}

#endif
