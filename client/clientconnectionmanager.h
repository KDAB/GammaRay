/*
  clientconnectionmanager.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CLIENTCONNECTIONMANAGER_H
#define GAMMARAY_CLIENTCONNECTIONMANAGER_H

#include "gammaray_client_export.h"

#include <QObject>
#include <QElapsedTimer>
#include <QUrl>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QMainWindow;
QT_END_NAMESPACE

namespace GammaRay {
class Client;
class MainWindow;
class ProcessTracker;
class ProcessTrackerBackend;
class ProcessTrackerInfo;
class ClientToolManager;

/*! Pre-MainWindow connection setup logic.
 *
 * This is useful for embedding the GammaRay client into another application
 *
 * @since 2.3
 */
class GAMMARAY_CLIENT_EXPORT ClientConnectionManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnectionManager(QObject *parent = nullptr, bool showSplashScreenOnStartUp = true);
    ~ClientConnectionManager() override;

    ClientToolManager *toolManager() const;
    QMainWindow *mainWindow() const;

    /*! Connect to a GammaRay probe at @p url. */
    void connectToHost(const QUrl &url, int tryAgain = 0);

    /*! Manually show the splash screen. */
    static void showSplashScreen();

    GammaRay::ProcessTrackerBackend *processTrackerBackend() const;
    void setProcessTrackerBackend(GammaRay::ProcessTrackerBackend *backend);

    qint64 processTrackerPid() const;
    void setProcessTrackerPid(qint64 pid);

    QString endPointLabel() const;
    QString endPointKey() const;
    qint64 endPointPid() const;

    /*! One-time initialization of stream operators and factory callbacks. */
    static void init();

signals:
    /*! Emitted when the connection is established and the tool model is populated.
     *  If you want to bring up the standard main window, connect this to createMainWindow(),
     *  otherwise use this to show your own UI at this point.
     */
    void ready();

    /*! Emitted when there has been a persistent connection error.
     *  You can connect this to handlePersistentConnectionError() for a standard
     *  message box and application exit handling.
     */
    void persistentConnectionError(const QString &msg);

    /*! Emitted when the connection to the target has been closed, for whatever reason.
     *  For a stand-alone client you probably want to connect this to QApplication::quit().
     */
    void disconnected();

    void processTrackerBackendChanged(GammaRay::ProcessTrackerBackend *backend);
    void processTrackerInfoChanged(const GammaRay::ProcessTrackerInfo &info);

public slots:
    /*! Disconnect GammaRay. */
    void disconnectFromHost();

    /*! Brings up a client main window for the current connection.
     *  If you want to use this, connect this slot to ready().
     */
    QMainWindow *createMainWindow();

    /*! Standard persistent connection error handler.
     *  @see persistentConnectionError()
     */
    void handlePersistentConnectionError(const QString &msg);

private slots:
    void doConnectToHost();
    void transientConnectionError();

    void delayedHideSplashScreen();
    static void hideSplashScreen();
    void targetQuitRequested();

    void updateProcessTrackerState();
    void clientConnected();
    void clientDisconnected();

private:
    QUrl m_serverUrl;
    Client *m_client;
    GammaRay::ProcessTracker *m_processTracker;
    ClientToolManager *m_toolManager;
    QPointer<MainWindow> m_mainWindow;
    QElapsedTimer m_connectionTimeout;
    bool m_ignorePersistentError;
    int m_tries;
};
}

#endif // GAMMARAY_CLIENTCONNECTIONMANAGER_H
