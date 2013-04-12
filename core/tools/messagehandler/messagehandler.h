/*
  messagehandler.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLER_H
#define GAMMARAY_MESSAGEHANDLER_MESSAGEHANDLER_H

#include "include/toolfactory.h"

#include <tools/messagehandler/messagehandlerwidget.h>

#include <QWidget>


namespace GammaRay {

class MessageModel;

namespace Ui {
  class MessageHandler;
}

class MessageHandler : public QObject
{
  Q_OBJECT
  public:
    explicit MessageHandler(ProbeInterface *probe, QObject *parent = 0);
    ~MessageHandler();

  private slots:
    void ensureHandlerInstalled();

  private:
    MessageModel *m_messageModel;
};

class MessageHandlerFactory : public QObject, public StandardToolFactory2<QObject, MessageHandler, MessageHandlerWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  public:
    explicit MessageHandlerFactory(QObject *parent);

    virtual inline QString name() const
    {
      return tr("Messages");
    }
};

}

#endif // MESSAGEHANDLER_H
