/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_ATTACHDIALOG_H
#define GAMMARAY_ATTACHDIALOG_H

#include <QWidget>
#include <memory>

#include "gammaray_launcher_ui_export.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class LaunchOptions;
class ProcessModel;
class ProcessFilterModel;
class ProbeABIModel;

namespace Ui {
    class AttachDialog;
}

/*! Widget for selecting a process to attach GammaRay to. */
class GAMMARAY_LAUNCHER_UI_EXPORT AttachDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AttachDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~AttachDialog() override;

    LaunchOptions launchOptions() const;
    int pid() const;
    QString absoluteExecutablePath() const;

    /// Returns @c true if a valid process is selected.
    bool isValid() const;
    void writeSettings();

    void setSettingsVisible(bool visible);

signals:
    void updateButtonState();
    void activate();

private slots:
    void updateProcesses();
    void updateProcessesFinished();
    void selectABI(const QModelIndex &processIndex);

private:
    std::unique_ptr<Ui::AttachDialog> ui;
    ProcessModel *m_model;
    ProcessFilterModel *m_proxyModel;
    ProbeABIModel *m_abiModel;
};
} // namespace GammaRay

#endif // ATTACHDIALOG_H
