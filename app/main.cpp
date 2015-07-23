/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <config-gammaray.h>

#include <common/paths.h>

#include <launcher/launcher.h>
#include <launcher/launchoptions.h>
#include <launcher/ui/launcherwindow.h>

#include <client/clientconnectionmanager.h>

#include <QApplication>
#include <QPointer>

using namespace GammaRay;

class InternalLauncher : public Launcher
{
    Q_OBJECT
public:
    explicit InternalLauncher(const LaunchOptions& options, QObject* parent = 0) : Launcher(options, parent) {}

signals:
    void launchClient(const QUrl &serverAddress);

protected:
    void startClient(const QUrl& serverAddress) Q_DECL_OVERRIDE
    {
        emit launchClient(serverAddress);
    }
};

class Orchestrator : public QObject
{
    Q_OBJECT
public:
    explicit Orchestrator(QObject *parent = 0) :
        QObject(parent)
    {
        m_launcherWindow = new LauncherWindow;
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
        connect(conMan, SIGNAL(persistentConnectionError(QString)), conMan, SLOT(handlePersistentConnectionError(QString)));
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


int main(int argc, char** argv)
{
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.com");
    QCoreApplication::setApplicationName("GammaRay");
    QApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);
    Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
    ClientConnectionManager::init();

    Orchestrator o;
    return app.exec();
}

#include "main.moc"
