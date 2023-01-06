/*
  launchpage.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LAUNCHPAGE_H
#define GAMMARAY_LAUNCHPAGE_H

#include <launcher/core/probeabidetector.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStringListModel;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeABIModel;

class LaunchOptions;
namespace Ui {
class LaunchPage;
}

class LaunchPage : public QWidget
{
    Q_OBJECT
public:
    explicit LaunchPage(QWidget *parent = nullptr);
    ~LaunchPage() override;

    LaunchOptions launchOptions() const;

    bool isValid();

    void writeSettings();

signals:
    void updateButtonState();

private slots:
    void showFileDialog();
    void showDirDialog();
    void addArgument();
    void removeArgument();
    void updateArgumentButtons();
    void detectABI(const QString &path);

private:
    bool fileIsExecutable() const;

    static QStringList notEmptyString(const QStringList &list);
    Ui::LaunchPage *ui;
    QStringListModel *m_argsModel;
    ProbeABIModel *m_abiModel;
    ProbeABIDetector m_abiDetector;
    bool m_abiIsValid;
};
}

#endif // GAMMARAY_LAUNCHPAGE_H
