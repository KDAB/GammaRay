/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include <config-gammaray-version.h>

#include <common/paths.h>

#include <launcher/core/launcher.h>
#include <launcher/core/launchoptions.h>
#include <launcher/ui/launcherwindow.h>

#include <client/clientconnectionmanager.h>

#include <QApplication>
#include <QPointer>

using namespace GammaRay;

class InternalLauncher : public Launcher
{
    Q_OBJECT
public:
    explicit InternalLauncher(const LaunchOptions &options, QObject *parent = nullptr)
        : Launcher(options, parent)
    {
    }

signals:
    void launchClient(const QUrl &serverAddress);

protected:
    void startClient(const QUrl &serverAddress) override
    {
        emit launchClient(serverAddress);
    }
};

class Orchestrator : public QObject
{
    Q_OBJECT
public:
    explicit Orchestrator(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_launcherWindow = new LauncherWindow;
        // For some reason, Qt4 on OSX does not respect setQuitOnLastWindowClosed(false)
        m_launcherWindow->setAttribute(Qt::WA_QuitOnClose, false);
        connect(m_launcherWindow, SIGNAL(accepted()), this, SLOT(launcherWindowAccepted()));
        connect(m_launcherWindow, SIGNAL(rejected()), QCoreApplication::instance(), SLOT(quit()));
        m_launcherWindow->show();
    }

public slots:
    void launcherWindowAccepted()
    {
        Q_ASSERT(m_launcherWindow);
        Q_ASSERT(m_launcherWindow->result() == QDialog::Accepted);
        const LaunchOptions opts = m_launcherWindow->launchOptions();

        if (!opts.isValid()) {
            QCoreApplication::exit(1);
            return;
        }

        m_launcher = new InternalLauncher(opts, this);
        connect(m_launcher, SIGNAL(launchClient(QUrl)), this, SLOT(startClient(QUrl)));
        connect(m_launcher, SIGNAL(finished()), this, SLOT(launcherFinished()));
        m_launcher->start();
    }

    void startClient(const QUrl &serverAddress)
    {
        auto *conMan = new ClientConnectionManager(this);
        connect(conMan, SIGNAL(ready()), conMan, SLOT(createMainWindow()));
        connect(conMan, SIGNAL(disconnected()), QApplication::instance(), SLOT(quit()));
        connect(conMan, SIGNAL(persistentConnectionError(QString)), conMan,
                SLOT(handlePersistentConnectionError(QString)));
        conMan->connectToHost(serverAddress);
    }

    void launcherFinished()
    {
        m_launcher->deleteLater();
    }

private:
    QPointer<LauncherWindow> m_launcherWindow;
    QPointer<Launcher> m_launcher;
};

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.com");
    QCoreApplication::setApplicationName("GammaRay");
    QCoreApplication::setApplicationVersion(GAMMARAY_COMPACT_VERSION_STRING);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);
    Paths::setRelativeRootPath(
#if defined(Q_OS_MACX) && defined(GAMMARAY_INSTALL_QT_LAYOUT)
        GAMMARAY_INVERSE_BUNDLE_DIR
#else
        GAMMARAY_INVERSE_BIN_DIR
#endif
    );
    ClientConnectionManager::init();

    Orchestrator o;
    return app.exec();
}

#include "main.moc"
