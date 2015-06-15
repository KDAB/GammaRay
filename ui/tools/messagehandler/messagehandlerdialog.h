/*
  messagehandlerdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MESSAGEHANDLERDIALOG_H
#define GAMMARAY_MESSAGEHANDLERDIALOG_H

#include <QDialog>

namespace GammaRay {

namespace Ui
{
class MessageHandlerDialog;
}

class MessageHandlerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MessageHandlerDialog(QWidget* parent = 0);
    ~MessageHandlerDialog();
    void setTitleData(const QString &app, const QTime &time);
    void setMessage(const QString &message);
    void setBacktrace(const QStringList &backtrace);
private slots:
    void copyBacktraceToClipboard();
private:
    QScopedPointer<Ui::MessageHandlerDialog> ui;
    QStringList m_backtrace;
};
}

#endif // GAMMARAY_MESSAGEHANDLERDIALOG_H
