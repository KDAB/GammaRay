/*
  clientconnectionmanager.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTCONNECTIONMANAGER_H
#define GAMMARAY_CLIENTCONNECTIONMANAGER_H

#include "gammaray_client_export.h"

#include <QObject>
#include <QTime>
#include <QUrl>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QMainWindow;
QT_END_NAMESPACE

namespace GammaRay {
class Client;
class MainWindow;

/** @brief Pre-MainWindow connection setup logic.
 *
 * This is useful for embedding the GammaRay client into another application
 *
 * @since 2.3
 */
class GAMMARAY_CLIENT_EXPORT ClientConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnectionManager(QObject *parent = 0, bool showSplashScreenOnStartUp = true);
    ~ClientConnectionManager();

    QMainWindow *mainWindow() const;

    /** Connect to a GammaRay probe at @p url. */
    void connectToHost(const QUrl &url, int tryAgain = 0);

    /** One-time initialization of stream operators and factory callbacks. */
    static void init();

signals:
    /** Emitted when the connection is established and the tool model is populated.
     *  If you want to bring up the standard main window, connect this to createMainWindow(),
     *  otherwise use this to show your own UI at this point.
     */
    void ready();

    /** Emitted when there has been a persistent connection error.
     *  You can connect this to handlePersistentConnectionError() for a standard
     *  message box and application exit handling.
     */
    void persistentConnectionError(const QString &msg);

    /** Emitted when the connection to the target has been closed, for whatever reason.
     *  For a stand-alone client you probably want to connect this to QApplication::quit().
     */
    void disconnected();

public slots:
    /** Disconnect GammaRay. */
    void disconnectFromHost();

    /** Brings up a client main window for the current connection.
     *  If you want to use this, connect this slot to ready().
     */
    QMainWindow *createMainWindow();

    /** Standard persistent connection error handler.
     *  @see persistentConnectionError()
     */
    void handlePersistentConnectionError(const QString &msg);

private slots:
    void connectToHost();
    void connectionEstablished();
    void transientConnectionError();

    void delayedHideSplashScreen();
    void hideSplashScreen();
    void targetQuitRequested();

private:
    QUrl m_serverUrl;
    Client *m_client;
    QPointer<MainWindow> m_mainWindow;
    QTime m_connectionTimeout;
    bool m_ignorePersistentError;
    int m_tries;
};
}

#endif // GAMMARAY_CLIENTCONNECTIONMANAGER_H
