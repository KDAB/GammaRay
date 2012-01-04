/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (info@qt.nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "qmlprofilertool.h"
#include "qmlprofilerconstants.h"
#include "qmlprofilereventview.h"

#include "tracewindow.h"
#include "timelineview.h"

#include <qmljsdebugclient/qdeclarativedebugclient.h>


#include "canvas/qdeclarativecanvas.h"
#include "canvas/qdeclarativecontext2d.h"
#include "canvas/qdeclarativetiledcanvas.h"




#include <QtCore/QFile>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QToolButton>
#include <QMessageBox>

using namespace QmlProfiler::Internal;
using namespace QmlJsDebugClient;

class QmlProfilerTool::QmlProfilerToolPrivate
{
public:
    QmlProfilerToolPrivate(QmlProfilerTool *qq) : q(qq) {}
    ~QmlProfilerToolPrivate() {}

    QmlProfilerTool *q;

    QDeclarativeDebugConnection *m_client;
    QTimer m_connectionTimer;
    int m_connectionAttempts;
    TraceWindow *m_traceWindow;
    QmlProfilerEventStatistics *m_statistics;
    QmlProfilerEventsView *m_eventsView;
    QmlProfilerEventsView *m_calleeView;
    QmlProfilerEventsView *m_callerView;
//     ProjectExplorer::Project *m_project;
//     Utils::FileInProjectFinder m_projectFinder;
//     ProjectExplorer::RunConfiguration *m_runConfiguration;
    bool m_isAttached;
    QAction *m_attachAction;
    QToolButton *m_recordButton;
    QToolButton *m_clearButton;
    bool m_recordingEnabled;

    enum ConnectMode {
        TcpConnection, OstConnection
    };

    ConnectMode m_connectMode;
    QString m_tcpHost;
    quint64 m_tcpPort;
    QString m_ostDevice;
};

QmlProfilerTool::QmlProfilerTool(QWidget *parent)
    : QWidget(parent), d(new QmlProfilerToolPrivate(this))
{
    setObjectName("QmlProfilerTool");
    d->m_client = 0;
    d->m_connectionAttempts = 0;
    d->m_traceWindow = 0;
    d->m_isAttached = false;
    d->m_attachAction = 0;
    d->m_recordingEnabled = true;

    d->m_connectMode = QmlProfilerToolPrivate::TcpConnection;

    d->m_connectionTimer.setInterval(200);
    connect(&d->m_connectionTimer, SIGNAL(timeout()), SLOT(tryToConnect()));

    qmlRegisterType<Canvas>("Monitor", 1, 0, "Canvas");
    qmlRegisterType<TiledCanvas>("Monitor", 1, 0, "TiledCanvas");
    qmlRegisterType<Context2D>();
    qmlRegisterType<CanvasImage>();
    qmlRegisterType<CanvasGradient>();
    qmlRegisterType<TimelineView>("Monitor", 1, 0,"TimelineView");
}

QmlProfilerTool::~QmlProfilerTool()
{
    delete d->m_client;
    delete d;
}

QByteArray QmlProfilerTool::id() const
{
    return "QmlProfiler";
}

QString QmlProfilerTool::displayName() const
{
    return tr("QML Profiler");
}

QString QmlProfilerTool::description() const
{
    return tr("The QML Profiler can be used to find performance bottlenecks in "
              "applications using QML.");
}


#if 0
IAnalyzerEngine *QmlProfilerTool::createEngine(const AnalyzerStartParameters &sp,
    ProjectExplorer::RunConfiguration *runConfiguration)
{
    QmlProfilerEngine *engine = new QmlProfilerEngine(this, runConfiguration);

    // Check minimum Qt Version. We cannot really be sure what the Qt version
    // at runtime is, but guess that the active build configuraiton has been used.
    QtSupport::QtVersionNumber minimumVersion(4, 7, 4);
    if (Qt4ProjectManager::Qt4BuildConfiguration *qt4Config
            = qobject_cast<Qt4ProjectManager::Qt4BuildConfiguration*>(
                runConfiguration->target()->activeBuildConfiguration())) {
        if (qt4Config->qtVersion()->isValid() && qt4Config->qtVersion()->qtVersion() < minimumVersion) {
            int result = QMessageBox::warning(QApplication::activeWindow(), tr("QML Profiler"),
                 "The QML profiler requires Qt 4.7.4 or newer.\n"
                 "The Qt version configured in your active build configuration is too old.\n"
                 "Do you want to continue?", QMessageBox::Yes, QMessageBox::No);
            if (result == QMessageBox::No)
                return 0;
        }
    }

    d->m_connectMode = QmlProfilerToolPrivate::TcpConnection;

    if (Qt4ProjectManager::S60DeployConfiguration *deployConfig
            = qobject_cast<Qt4ProjectManager::S60DeployConfiguration*>(
                runConfiguration->target()->activeDeployConfiguration())) {
        if (deployConfig->communicationChannel()
                == Qt4ProjectManager::S60DeployConfiguration::CommunicationCodaSerialConnection) {
            d->m_connectMode = QmlProfilerToolPrivate::OstConnection;
            d->m_ostDevice = deployConfig->serialPortName();
        }
    }

    // FIXME: Check that there's something sensible in sp.connParams
    if (d->m_connectMode == QmlProfilerToolPrivate::TcpConnection) {
        d->m_tcpHost = sp.connParams.host;
        d->m_tcpPort = sp.connParams.port;
    }

    d->m_runConfiguration = runConfiguration;
    d->m_project = runConfiguration->target()->project();
    if (d->m_project) {
        d->m_projectFinder.setProjectDirectory(d->m_project->projectDirectory());
        updateProjectFileList();
        connect(d->m_project, SIGNAL(fileListChanged()), this, SLOT(updateProjectFileList()));
    }

    connect(engine, SIGNAL(processRunning(int)), this, SLOT(connectClient(int)));
    connect(engine, SIGNAL(finished()), this, SLOT(disconnectClient()));
    connect(engine, SIGNAL(stopRecording()), this, SLOT(stopRecording()));
    connect(d->m_traceWindow, SIGNAL(viewUpdated()), engine, SLOT(dataReceived()));
    connect(this, SIGNAL(connectionFailed()), engine, SLOT(finishProcess()));
    connect(this, SIGNAL(fetchingData(bool)), engine, SLOT(setFetchingData(bool)));
    emit fetchingData(d->m_recordButton->isChecked());

    return engine;
}
#endif

void QmlProfilerTool::toolSelected()
{
    updateAttachAction(true);
}

void QmlProfilerTool::toolDeselected()
{
    updateAttachAction(false);
}

QWidget *QmlProfilerTool::createWidgets()
{
//     Analyzer::AnalyzerManager *analyzerMgr = Analyzer::AnalyzerManager::instance();
//     Utils::FancyMainWindow *mw = analyzerMgr->mainWindow();

    QWidget *mw = this;
    d->m_traceWindow = new TraceWindow(mw);
    d->m_traceWindow->reset(d->m_client);

    connect(d->m_traceWindow, SIGNAL(gotoSourceLocation(QString,int)),this, SLOT(gotoSourceLocation(QString,int)));
    connect(d->m_traceWindow, SIGNAL(timeChanged(qreal)), this, SLOT(updateTimer(qreal)));

    d->m_statistics = new QmlProfilerEventStatistics(mw);
    d->m_eventsView = new QmlProfilerEventsView(mw, d->m_statistics);
    d->m_eventsView->setViewType(QmlProfilerEventsView::EventsView);

    connect(d->m_traceWindow, SIGNAL(range(int,int,int,qint64,qint64,QStringList,QString,int)),
            d->m_statistics, SLOT(addRangedEvent(int,int,int,qint64,qint64,QStringList,QString,int)));
    connect(d->m_traceWindow, SIGNAL(viewUpdated()),
            d->m_statistics, SLOT(complete()));
    connect(d->m_eventsView, SIGNAL(gotoSourceLocation(QString,int)),
            this, SLOT(gotoSourceLocation(QString,int)));

    d->m_calleeView = new QmlProfilerEventsView(mw, d->m_statistics);
    d->m_calleeView->setViewType(QmlProfilerEventsView::CalleesView);
    connect(d->m_calleeView, SIGNAL(gotoSourceLocation(QString,int)),
            this, SLOT(gotoSourceLocation(QString,int)));

    d->m_callerView = new QmlProfilerEventsView(mw, d->m_statistics);
    d->m_callerView->setViewType(QmlProfilerEventsView::CallersView);
    connect(d->m_callerView, SIGNAL(gotoSourceLocation(QString,int)),
            this, SLOT(gotoSourceLocation(QString,int)));

#if 0
    Core::ICore *core = Core::ICore::instance();
    Core::ActionManager *am = core->actionManager();
    Core::ActionContainer *manalyzer = am->actionContainer(Analyzer::Constants::M_DEBUG_ANALYZER);
    const Core::Context globalcontext(Core::Constants::C_GLOBAL);

    d->m_attachAction = new QAction(tr("Attach..."), manalyzer);
    Core::Command *command = am->registerAction(d->m_attachAction,
                                                Constants::ATTACH, globalcontext);
    command->setAttribute(Core::Command::CA_UpdateText);
    //manalyzer->addAction(command, Analyzer::Constants::G_ANALYZER_STARTSTOP);
    connect(d->m_attachAction, SIGNAL(triggered()), this, SLOT(attach()));

    updateAttachAction(false);

    QDockWidget *eventsDock = AnalyzerManager::createDockWidget
            (this, tr("Events"), d->m_eventsView, Qt::BottomDockWidgetArea);
    QDockWidget *timelineDock = AnalyzerManager::createDockWidget
            (this, tr("Timeline"), d->m_traceWindow, Qt::BottomDockWidgetArea);
    QDockWidget *calleeDock = AnalyzerManager::createDockWidget
            (this, tr("Callees"), d->m_calleeView, Qt::BottomDockWidgetArea);
    QDockWidget *callerDock = AnalyzerManager::createDockWidget
            (this, tr("Callers"), d->m_callerView, Qt::BottomDockWidgetArea);

    mw->splitDockWidget(mw->toolBarDockWidget(), eventsDock, Qt::Vertical);
    mw->tabifyDockWidget(eventsDock, timelineDock);
    mw->tabifyDockWidget(timelineDock, calleeDock);
    mw->tabifyDockWidget(calleeDock, callerDock);
#endif
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(d->m_eventsView, tr("Events"));
    tabWidget->addTab(d->m_traceWindow, tr("Timeline"));
    tabWidget->addTab(d->m_calleeView, tr("Callees"));
    tabWidget->addTab(d->m_callerView, tr("Callers"));

    //
    // Toolbar
    //
    QWidget *toolbarWidget = new QWidget;
    toolbarWidget->setObjectName(QLatin1String("QmlProfilerToolBarWidget"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    d->m_recordButton = new QToolButton(toolbarWidget);
    // icon and tooltip set in setRecording(), called later
    d->m_recordButton->setCheckable(true);

    connect(d->m_recordButton,SIGNAL(toggled(bool)), this, SLOT(setRecording(bool)));
    d->m_recordButton->setChecked(true);
    layout->addWidget(d->m_recordButton);

    d->m_clearButton = new QToolButton(toolbarWidget);
    d->m_clearButton->setIcon(QIcon(QLatin1String(":/qmlprofiler/clean_pane_small.png")));
    d->m_clearButton->setToolTip(tr("Discard data"));
    connect(d->m_clearButton,SIGNAL(clicked()), this, SLOT(clearDisplay()));
    layout->addWidget(d->m_clearButton);

    QLabel *timeLabel = new QLabel(tr("Elapsed:      0 s"));
    QPalette palette = timeLabel->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    timeLabel->setPalette(palette);
    timeLabel->setIndent(10);

    connect(this, SIGNAL(setTimeLabel(QString)), timeLabel, SLOT(setText(QString)));
    layout->addWidget(timeLabel);

    toolbarWidget->setLayout(layout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolbarWidget);
    mainLayout->addWidget(tabWidget);

    return toolbarWidget;
}

void QmlProfilerTool::connectClient(int port)
{
//     QTC_ASSERT(!d->m_client, return;)
    d->m_client = new QDeclarativeDebugConnection;
    d->m_traceWindow->reset(d->m_client);
    connect(d->m_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(connectionStateChanged()));
    d->m_connectionTimer.start();
    d->m_tcpPort = port;
    d->m_tcpHost = "localhost";
}

void QmlProfilerTool::connectToClient()
{
    if (!d->m_client || d->m_client->state() != QAbstractSocket::UnconnectedState)
        return;

    if (d->m_connectMode == QmlProfilerToolPrivate::TcpConnection) {
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: Connecting to %s:%lld ...", qPrintable(d->m_tcpHost), d->m_tcpPort);

        d->m_client->connectToHost(d->m_tcpHost, d->m_tcpPort);
    } else {
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: Connecting to ost device %s...", qPrintable(d->m_ostDevice));

        d->m_client->connectToOst(d->m_ostDevice);
    }
}

void QmlProfilerTool::disconnectClient()
{
    // this might be actually be called indirectly by QDDConnectionPrivate::readyRead(), therefore allow
    // method to complete before deleting object
    if (d->m_client) {
        d->m_client->deleteLater();
        d->m_client = 0;
    }
}

void QmlProfilerTool::startRecording()
{
    if (d->m_client && d->m_client->isConnected()) {
        clearDisplay();
        d->m_traceWindow->setRecording(true);
    }
    emit fetchingData(true);
}

void QmlProfilerTool::stopRecording()
{
    d->m_traceWindow->setRecording(false);
    emit fetchingData(false);
}

void QmlProfilerTool::setRecording(bool recording)
{
    d->m_recordingEnabled = recording;

    // update record button
    d->m_recordButton->setToolTip( d->m_recordingEnabled ? tr("Disable profiling") : tr("Enable profiling"));
    d->m_recordButton->setIcon(QIcon(d->m_recordingEnabled ? QLatin1String(":/qmlprofiler/recordOn.png") :
                                                             QLatin1String(":/qmlprofiler/recordOff.png")));

    if (recording)
        startRecording();
    else
        stopRecording();
}

void QmlProfilerTool::gotoSourceLocation(const QString &fileUrl, int lineNumber)
{
    if (lineNumber < 0 || fileUrl.isEmpty())
        return;

#if 0
    const QString fileName = QUrl(fileUrl).toLocalFile();
    const QString projectFileName = d->m_projectFinder.findFile(fileName);

    Core::EditorManager *editorManager = Core::EditorManager::instance();
    Core::IEditor *editor = editorManager->openEditor(projectFileName);
    TextEditor::ITextEditor *textEditor = qobject_cast<TextEditor::ITextEditor*>(editor);

    if (textEditor) {
        editorManager->addCurrentPositionToNavigationHistory();
        textEditor->gotoLine(lineNumber);
        textEditor->widget()->setFocus();
    }
#endif
}

void QmlProfilerTool::updateTimer(qreal elapsedSeconds)
{
    QString timeString = QString::number(elapsedSeconds,'f',1);
    timeString = QString("      ").left(6-timeString.length()) + timeString;
    emit setTimeLabel(tr("Elapsed: %1 s").arg(timeString));
}

void QmlProfilerTool::updateProjectFileList()
{
#if 0
    d->m_projectFinder.setProjectFiles(
                d->m_project->files(ProjectExplorer::Project::ExcludeGeneratedFiles));
#endif
}

void QmlProfilerTool::clearDisplay()
{
    d->m_traceWindow->clearDisplay();
    d->m_statistics->clear();
    d->m_eventsView->clear();
    d->m_calleeView->clear();
    d->m_callerView->clear();
}

void QmlProfilerTool::attach()
{
    if (!d->m_isAttached) {
//         QmlProfilerAttachDialog dialog;
//         int result = dialog.exec();

//         if (result == QDialog::Rejected)
//             return;

// TODO
//         d->m_tcpPort = dialog.port();
//         d->m_tcpHost = dialog.address();

        connectClient(d->m_tcpPort);
//         AnalyzerManager::showMode();
    } else {
        stopRecording();
    }

    d->m_isAttached = !d->m_isAttached;
    updateAttachAction(true);
}

void QmlProfilerTool::updateAttachAction(bool isCurrentTool)
{
    if (d->m_isAttached)
        d->m_attachAction->setText(tr("Detach"));
    else
        d->m_attachAction->setText(tr("Attach..."));
    d->m_attachAction->setEnabled(isCurrentTool);
}

void QmlProfilerTool::tryToConnect()
{
    ++d->m_connectionAttempts;

    if (d->m_client && d->m_client->isConnected()) {
        d->m_connectionTimer.stop();
        d->m_connectionAttempts = 0;
    } else if (d->m_connectionAttempts == 50) {
        d->m_connectionTimer.stop();
        d->m_connectionAttempts = 0;
//         if (QmlProfilerPlugin::debugOutput) {
//             if (d->m_client) {
//                 qWarning("QML Profiler: Failed to connect: %s", qPrintable(d->m_client->errorString()));
// //             } else {
//                 qWarning("QML Profiler: Failed to connect.");
//             }
//         }
        emit connectionFailed();
    } else {
        connectToClient();
    }
}

void QmlProfilerTool::connectionStateChanged()
{
    if (!d->m_client)
        return;
    switch (d->m_client->state()) {
    case QAbstractSocket::UnconnectedState:
    {
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: disconnected");
        break;
    }
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ConnectingState: {
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: Connecting to debug server ...");
        break;
    }
    case QAbstractSocket::ConnectedState:
    {
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: connected and running");
        updateRecordingState();
        break;
    }
    case QAbstractSocket::ClosingState:
//         if (QmlProfilerPlugin::debugOutput)
//             qWarning("QML Profiler: closing ...");
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
        break;
    }
}

void QmlProfilerTool::updateRecordingState()
{
    if (d->m_client->isConnected()) {
        d->m_traceWindow->setRecording(d->m_recordingEnabled);
    } else {
        d->m_traceWindow->setRecording(false);
    }

    if (d->m_traceWindow->isRecording())
        clearDisplay();
}

#if 0
void QmlProfilerTool::startTool(StartMode mode)
{
    Q_UNUSED(mode);

    using namespace ProjectExplorer;

    // Make sure mode is shown.
    AnalyzerManager::showMode();

    ProjectExplorerPlugin *pe = ProjectExplorerPlugin::instance();
    // ### not sure if we're supposed to check if the RunConFiguration isEnabled
    Project *pro = pe->startupProject();
    pe->runProject(pro, id());
}
#endif

#include "qmlprofilertool.moc"
