/*
  attachdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
