/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef ATTACHDIALOG_H
#define ATTACHDIALOG_H

#include <QDialog>

#include "ui_attachdialog.h"

namespace GammaRay {

class ProcessModel;
class ProcessFilterModel;

class AttachDialog : public QDialog
{
  Q_OBJECT

  public:
    explicit AttachDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    QString pid() const;

  private slots:
    void selectionChanged();
    void updateProcesses();
    void updateProcessesFinished();

  private:
    Ui::AttachDialog ui;
    ProcessModel *m_model;
    ProcessFilterModel *m_proxyModel;
    QTimer *m_timer;
};

} // namespace GammaRay

#endif // ATTACHDIALOG_H
