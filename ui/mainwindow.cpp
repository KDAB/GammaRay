/*
  mainwindow.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "aboutpluginsdialog.h"
#include "aboutdialog.h"
#include "clienttoolmanager.h"
#include "clienttoolfilterproxymodel.h"
#include "aboutdata.h"
#include "uiintegration.h"
#include "helpcontroller.h"
#include "uiresources.h"

#include "common/objectbroker.h"
#include "common/modelroles.h"
#include "common/endpoint.h"
#include "common/probecontrollerinterface.h"

#include "kde/klinkitemselectionmodel.h"

#ifndef GAMMARAY_DISABLE_FEEDBACK
#include <3rdparty/kuserfeedback/widgets/feedbackconfigdialog.h>
#include <3rdparty/kuserfeedback/widgets/notificationpopup.h>
#include <3rdparty/kuserfeedback/core/applicationversionsource.h>
#include <3rdparty/kuserfeedback/core/compilerinfosource.h>
#include <3rdparty/kuserfeedback/core/openglinfosource.h>
#include <3rdparty/kuserfeedback/core/platforminfosource.h>
#include <3rdparty/kuserfeedback/core/provider.h>
#include <3rdparty/kuserfeedback/core/qtversionsource.h>
#include <3rdparty/kuserfeedback/core/selectionratiosource.h>
#include <3rdparty/kuserfeedback/core/startcountsource.h>
#include <3rdparty/kuserfeedback/core/usagetimesource.h>
#endif

#include <qpa/qplatformtheme.h>
#include <private/qguiapplication_p.h>

#include <QAction>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QStyleFactory>
#include <QTableView>
#include <QToolButton>
#include <QUrl>
#include <QWidgetAction>

#include <QStandardPaths>

#include <iostream>

using namespace GammaRay;

namespace {

struct IdeSettings {
    const char * const app;
    const char * const args;
    const char * const name;
    const char * const icon;
};

static const IdeSettings ideSettings[] = {
#if defined(Q_OS_WIN) || defined(Q_OS_OSX)
    {"", "", "", ""          }                                                          // Dummy content, because we can't have empty arrays.
#else
    { "kdevelop", "%f:%l:%c", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "KDevelop"), "kdevelop"  },
    { "kate", "%f --line %l --column %c", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "Kate"),
      "kate"      },
    { "kwrite", "%f --line %l --column %c", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "KWrite"),
      nullptr     },
    { "gedit", "%f +%l:%c", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "gedit"),
      nullptr     },
    { "gvim", "%f +%l", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "gvim"),
      nullptr     },
    { "qtcreator", "%f:%l:%c", QT_TRANSLATE_NOOP("GammaRay::MainWindow", "Qt Creator"), nullptr     }
#endif
};
#if defined(Q_OS_WIN) || defined(Q_OS_OSX) // Remove this #if branch when adding real data to ideSettings for Windows/OSX.
static const int ideSettingsSize = 0;
#else
static const int ideSettingsSize = sizeof(ideSettings) / sizeof(IdeSettings);
#endif

QStyle *gammarayStyleOverride()
{
    const auto styleNameOverride = QString::fromLocal8Bit(qgetenv("GAMMARAY_STYLE"));
    if (styleNameOverride.isEmpty()) {
        return nullptr;
    }

    if (!QStyleFactory::keys().contains(styleNameOverride)) {
        qWarning() << "Style" << styleNameOverride << "does not exit (enabled by GAMMARAY_STYLE environment variable)";
        qWarning() << "Existing styles: " << QStyleFactory::keys();
    }

    return QStyleFactory::create(styleNameOverride);
}

QStyle *gammarayDefaultStyle()
{
    foreach (const QString &styleName,
             QGuiApplicationPrivate::platform_theme->themeHint(QPlatformTheme::StyleNames).
             toStringList()) {
        if (auto style = QStyleFactory::create(styleName)) {
            return style;
        }
    }
    return nullptr;
}
}

MainWindowUIStateManager::MainWindowUIStateManager(QWidget *widget)
    : UIStateManager(widget)
{
}

QList<QSplitter *> MainWindowUIStateManager::splitters() const
{
    return QList<QSplitter *>();
}

QList<QHeaderView *> MainWindowUIStateManager::headers() const
{
    return QList<QHeaderView *>();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stateManager(this)
    , m_feedbackProvider(nullptr)
{
    const auto styleOverride = gammarayStyleOverride();
    if (styleOverride) {
        applyStyle(styleOverride);
    }

    if (!Endpoint::instance()->isRemoteClient()) {
        // we don't want application styles to propagate to the GammaRay window,
        // so set the platform default one if needed

        // check if the style is not already overwritten
        if (!styleOverride) {
            if (auto defaultStyle= gammarayDefaultStyle()) {
                applyStyle(defaultStyle);
            }
        }
    }

    UIResources::setTheme(UiIntegration::hasDarkUI() ? UIResources::Light : UIResources::Dark);

    ui->setupUi(this);

    connect(ui->actionRetractProbe, &QAction::triggered, this, &MainWindow::detachProbe);

    connect(QApplication::instance(), &QCoreApplication::aboutToQuit, this, &QWidget::close);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::quitHost);
    ui->actionQuit->setIcon(QIcon::fromTheme(QStringLiteral("application-exit")));

    ui->actionHelp->setShortcut(QKeySequence::HelpContents);
    ui->actionHelp->setEnabled(HelpController::isAvailable());

    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::help);
    connect(ui->actionPlugins, &QAction::triggered,
            this, &MainWindow::aboutPlugins);
    connect(ui->actionMessageStatistics, &QAction::triggered, this, &MainWindow::showMessageStatistics);
    connect(ui->actionAboutQt, &QAction::triggered,
            qobject_cast<QApplication*>(QApplication::instance()), &QApplication::aboutQt);
    connect(ui->actionAboutGammaRay, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAboutKDAB, &QAction::triggered, this, &MainWindow::aboutKDAB);

    setWindowIcon(QIcon(QStringLiteral(":/gammaray/GammaRay-128x128.png")));

    // ClientConnectionManager take care of creating and requesting server tools
    // but in-process ui need to do it itself.
    ClientToolManager *toolManager = ClientToolManager::instance();
    if (!toolManager) {
        toolManager = new ClientToolManager(this);
        toolManager->requestAvailableTools();
    }

    toolManager->setToolParentWidget(this);

    auto sourceModel = toolManager->model();
    auto sourceSelectionModel = toolManager->selectionModel();
    m_toolFilterModel = new ClientToolFilterProxyModel(this);
    m_toolFilterModel->setSourceModel(sourceModel);
    m_toolFilterModel->setDynamicSortFilter(true);
    m_toolFilterModel->sort(0);
    ui->toolSelector->setModel(m_toolFilterModel);
    ui->toolSelector->setSelectionModel(new KLinkItemSelectionModel(m_toolFilterModel, sourceSelectionModel, sourceModel));
    ui->toolSelector->resize(ui->toolSelector->minimumSize());
    connect(toolManager->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::toolSelected);
    connect(ui->toolSelector, &QWidget::customContextMenuRequested, this, &MainWindow::toolContextMenu);

    QSettings settings;
    settings.beginGroup("Sidebar");
    m_toolFilterModel->setFilterInactiveTools(settings.value(QLatin1String("FilterInactive"), false).toBool());
    settings.endGroup();

    // hide unused tool bar for now
    ui->mainToolBar->setHidden(true);

    setWindowTitle(tr("GammaRay (%1)").arg(Endpoint::instance()->label()));

    // Code Navigation
    QAction *configAction =
        new QAction(QIcon::fromTheme(QStringLiteral("applications-development")),
                    tr("Code Navigation"), this);
    auto menu = new QMenu(this);
    auto group = new QActionGroup(this);
    group->setExclusive(true);

    settings.beginGroup(QStringLiteral("CodeNavigation"));
    const auto currentIdx = settings.value(QStringLiteral("IDE"), -1).toInt();

    for (int i = 0; i < ideSettingsSize; ++i) {
        auto action = new QAction(menu);
        action->setText(tr(ideSettings[i].name));
        if (ideSettings[i].icon) {
            action->setIcon(QIcon::fromTheme(ideSettings[i].icon));
        }
        action->setCheckable(true);
        action->setChecked(currentIdx == i);
        action->setData(i);
        action->setEnabled(!QStandardPaths::findExecutable(ideSettings[i].app).isEmpty());
        group->addAction(action);
        menu->addAction(action);
    }
    menu->addSeparator();

    auto *action = new QAction(menu);
    action->setText(tr("Custom..."));
    action->setCheckable(true);
    action->setChecked(currentIdx == -1);
    action->setData(-1);
    group->addAction(action);
    menu->addAction(action);

#if defined(Q_OS_WIN) || defined(Q_OS_OSX)
    // This is a workaround for the cases, where we can't safely do assumptions
    // about the install location of the IDE
    action = new QAction(menu);
    action->setText(tr("Automatic (No Line numbers)"));
    action->setCheckable(true);
    action->setChecked(currentIdx == -2);
    action->setData(-2);
    group->addAction(action);
    menu->addAction(action);
#endif

    QObject::connect(group, &QActionGroup::triggered,
                     this, &MainWindow::setCodeNavigationIDE);

    configAction->setMenu(menu);
    ui->menuSettings->addMenu(menu);

    // Initialize UiIntegration singleton
    new UiIntegration(this);

    connect(UiIntegration::instance(), &UiIntegration::navigateToCode, this,
            &MainWindow::navigateToCode);

    const bool developerModeEnabled = !qgetenv("GAMMARAY_DEVELOPERMODE").isEmpty();
    if (developerModeEnabled) {
        connect(Endpoint::instance(), &Endpoint::logTransmissionRate,
                this, &MainWindow::logTransmissionRate);
    } else {
        ui->statusBar->hide();
        ui->menu_Diagnostics->menuAction()->setVisible(false);
    }

    connect(this, &MainWindow::targetQuitRequested, &m_stateManager, &UIStateManager::saveState);
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.beginGroup("Sidebar");
    settings.setValue(QLatin1String("FilterInactive"), m_toolFilterModel->filterInactiveTools());
    settings.endGroup();
}

void MainWindow::saveTargetState(QSettings *settings) const
{
    // When closing/quitting the client main window, this is triggered but the connection is already closed
    if (Endpoint::instance()->isConnected()) {
        const QModelIndex toolIndex = ui->toolSelector->selectionModel()->selectedRows().value(0);
        const QString toolId = toolIndex.data(ToolModelRole::ToolId).toString();
        settings->setValue("selectedToolId", toolId);
    }
}

void MainWindow::restoreTargetState(QSettings *settings)
{
    Q_ASSERT(ClientToolManager::instance()->isToolListLoaded());
    const QString toolId = settings->value("selectedToolId", QStringLiteral("GammaRay::ObjectInspector")).toString();
    selectTool(toolId);
}

void MainWindow::setupFeedbackProvider()
{
#ifndef GAMMARAY_DISABLE_FEEDBACK
    ui->actionContribute->setEnabled(true);
    connect(ui->actionContribute, &QAction::triggered, this, &MainWindow::configureFeedback);
    m_feedbackProvider = new KUserFeedback::Provider(this);
    m_feedbackProvider->setProductIdentifier(QStringLiteral("com.kdab.GammaRay"));
    m_feedbackProvider->setFeedbackServer(QUrl(QStringLiteral("https://gammaray-userfeedback.kdab.com/")));
    m_feedbackProvider->setSubmissionInterval(7);
    m_feedbackProvider->setApplicationStartsUntilEncouragement(5);
    m_feedbackProvider->setEncouragementDelay(30);
    m_feedbackProvider->addDataSource(new KUserFeedback::ApplicationVersionSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::CompilerInfoSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::PlatformInfoSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::QtVersionSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::StartCountSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::UsageTimeSource);
    m_feedbackProvider->addDataSource(new KUserFeedback::OpenGLInfoSource);
    auto toolRatioSrc = new KUserFeedback::SelectionRatioSource(ui->toolSelector->selectionModel(), QStringLiteral("toolRatio"));
    toolRatioSrc->setDescription(tr("Usage ratio of the GammaRay tools."));
    toolRatioSrc->setRole(ToolModelRole::ToolFeedbackId);
    toolRatioSrc->setTelemetryMode(KUserFeedback::Provider::DetailedUsageStatistics);
    m_feedbackProvider->addDataSource(toolRatioSrc);

    auto popup = new KUserFeedback::NotificationPopup(this);
    popup->setFeedbackProvider(m_feedbackProvider);
#endif
}

void MainWindow::help()
{
    HelpController::openContents();
}

void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.setWindowTitle(tr("About GammaRay"));
    dialog.setWindowIcon(QPixmap(QStringLiteral(":/gammaray/GammaRay-128x128.png")));
    dialog.setThemeLogo(QStringLiteral("gammaray-trademark.png"));
    dialog.setTitle(AboutData::aboutTitle());
    dialog.setHeader(AboutData::aboutHeader());
    dialog.setAuthors(AboutData::aboutAuthors());
    dialog.setFooter(AboutData::aboutFooter());
    dialog.adjustSize();
    dialog.exec();
}

void MainWindow::aboutPlugins()
{
    AboutPluginsDialog dlg(this);
    dlg.setFixedSize(800, 600);
    dlg.exec();
}

void MainWindow::aboutKDAB()
{
    AboutDialog dialog(this);
    dialog.setWindowTitle(tr("About KDAB"));
    dialog.setWindowIcon(UIResources::themedPixmap(QStringLiteral("kdab-logo.png"), this));
    dialog.setThemeLogo(QStringLiteral("kdab-logo.png"));
    dialog.setTitle(trUtf8("Klarälvdalens Datakonsult AB (KDAB)"));
    dialog.setText(
        tr("<qt><p>GammaRay is supported and maintained by KDAB</p>"
           "KDAB, the Qt experts, provide consulting and mentoring for developing "
           "Qt applications from scratch and in porting from all popular and legacy "
           "frameworks to Qt. We continue to help develop parts of Qt and are one "
           "of the major contributors to the Qt Project. We can give advanced or "
           "standard trainings anywhere around the globe.</p>"
           "<p>Please visit <a href='https://www.kdab.com'>https://www.kdab.com</a> "
           "to meet the people who write code like this."
           "</p></qt>"));
    dialog.exec();
}

void MainWindow::showMessageStatistics()
{
    auto view = new QTableView;
    view->setWindowTitle(tr("Communication Message Statistics"));
    view->setAttribute(Qt::WA_DeleteOnClose);
    view->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MessageStatisticsModel")));
    view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    view->showMaximized();
}

bool MainWindow::selectTool(const QString &id)
{
    if (id.isEmpty())
        return false;

    const QItemSelectionModel::SelectionFlags selectionFlags =
        QItemSelectionModel::ClearAndSelect |
        QItemSelectionModel::Rows |
        QItemSelectionModel::Current;
    const Qt::MatchFlags matchFlags = Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap;
    const QAbstractItemModel *model = ui->toolSelector->model();
    const QModelIndex toolIndex =
        model->match(model->index(0, 0), ToolModelRole::ToolId, id, 1, matchFlags).value(0);
    if (!toolIndex.isValid())
        return false;

    QItemSelectionModel *selectionModel = ui->toolSelector->selectionModel();
    selectionModel->setCurrentIndex(toolIndex, selectionFlags);
    return true;
}

void MainWindow::toolSelected()
{
    ui->actionsMenu->clear();
    QModelIndexList list = ui->toolSelector->selectionModel()->selectedRows();
    int row = -1;

    if (!list.isEmpty())
        row = list[0].row();

    if (row == -1)
        return;

    const QModelIndex mi = ui->toolSelector->model()->index(row, 0);
    QWidget *toolWidget = mi.data(ToolModelRole::ToolWidget).value<QWidget *>();
    if (!toolWidget) {
        toolWidget = createErrorPage(mi);
        ui->toolSelector->model()->setData(mi, QVariant::fromValue(
                                               toolWidget), ToolModelRole::ToolWidget);
    }

    Q_ASSERT(toolWidget);
    if (ui->toolStack->indexOf(toolWidget) < 0) { // newly created
        if (toolWidget->layout()) {
#ifndef Q_OS_MAC
            toolWidget->layout()->setContentsMargins(11, 0, 0, 0);
#else
            QMargins margins = toolWidget->layout()->contentsMargins();
            margins.setLeft(0);
            toolWidget->layout()->setContentsMargins(margins);
#endif
        }
        ui->toolStack->addWidget(toolWidget);
    }

    ui->toolStack->setCurrentIndex(ui->toolStack->indexOf(toolWidget));

    foreach (QAction *action, toolWidget->actions()) {
        if (auto widgetAction = qobject_cast<QWidgetAction *>(action)) {
            if (auto toolButton = qobject_cast<QToolButton *>(widgetAction->defaultWidget())) {
                auto subMenu = ui->actionsMenu->addMenu(toolButton->text());
                if (auto defaultAction = toolButton->defaultAction()) {
                    subMenu->addAction(defaultAction);
                    subMenu->addSeparator();
                }
                subMenu->addActions(toolButton->menu()->actions());
            }
        } else {
            ui->actionsMenu->addAction(action);
        }
    }
    ui->actionsMenu->setEnabled(!ui->actionsMenu->isEmpty());
    ui->actionsMenu->setTitle(mi.data().toString());
}

void MainWindow::toolContextMenu(QPoint pos)
{
    QMenu menu;
    auto action = menu.addAction(tr("Hide inactive tools"));
    action->setCheckable(true);
    action->setChecked(m_toolFilterModel->filterInactiveTools());
    connect(action, &QAction::toggled, m_toolFilterModel, &ClientToolFilterProxyModel::setFilterInactiveTools);
    menu.exec(ui->toolSelector->viewport()->mapToGlobal(pos));
}

void MainWindow::navigateToCode(const QUrl &url, int lineNumber, int columnNumber)
{
    // Show Qt resources in our qrc browser
    if (url.scheme() == "qrc") {
        if (selectTool(QStringLiteral("GammaRay::ResourceBrowser"))) {
            QMetaObject::invokeMethod(ui->toolStack->currentWidget(), "selectResource",
                                      Q_ARG(QString, url.toString()),
                                      Q_ARG(int, lineNumber), Q_ARG(int, columnNumber));
        }
    } else {
        QSettings settings;
        settings.beginGroup(QStringLiteral("CodeNavigation"));
        const auto ideIdx = settings.value(QStringLiteral("IDE"), -1).toInt();

        QString command;
#if !defined(Q_OS_WIN) && !defined(Q_OS_OSX) // Remove this #if branch when adding real data to ideSettings for Windows/OSX.
        if (ideIdx >= 0 && ideIdx < ideSettingsSize) {
            command += ideSettings[ideIdx].app;
            command += ' ';
            command += ideSettings[ideIdx].args;
        } else
#endif
        if (ideIdx == -1) {
            command = settings.value(QStringLiteral("CustomCommand")).toString();
        } else {
            QDesktopServices::openUrl(QUrl(url));
        }

        const QString filePath = url.isLocalFile() ? url.toLocalFile() : url.toString();
        command.replace(QStringLiteral("%f"), filePath);
        command.replace(QStringLiteral("%l"), QString::number(std::max(1, lineNumber + 1)));
        command.replace(QStringLiteral("%c"), QString::number(std::max(1, columnNumber + 1)));

        if (!command.isEmpty()) {
            std::cout << "Detaching: " << qPrintable(command) << std::endl;
            QProcess::startDetached(command);
        }
    }
}

void MainWindow::logTransmissionRate(quint64 bytesRead, quint64 bytesWritten)
{
    const double transmissionRateRX = (bytesRead * 8 / 1024.0 / 1024.0); // in Mpbs
    const double transmissionRateTX = (bytesWritten * 8 / 1024.0 / 1024.0); // in Mpbs
    ui->statusBar->showMessage(
        tr("Transmission rate: RX %1 Mbps, TX %2 Mbps").
            arg(transmissionRateRX, 7, 'f', 3).
            arg(transmissionRateTX, 7, 'f', 3));
}

void GammaRay::MainWindow::setCodeNavigationIDE(QAction *action)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("CodeNavigation"));

    if (action->data() == -1) {
        const auto customCmd = QInputDialog::getText(
            this, tr("Custom Code Navigation"),
            tr(
                "Specify command to use for code navigation, '%f' will be replaced by the file name, '%l' by the line number and '%c' by the column number."),
            QLineEdit::Normal, settings.value(QStringLiteral("CustomCommand")).toString()
            );
        if (!customCmd.isEmpty()) {
            settings.setValue(QStringLiteral("CustomCommand"), customCmd);
            settings.setValue(QStringLiteral("IDE"), -1);
        }
        return;
    }

    const auto defaultIde = action->data().toInt();
    settings.setValue(QStringLiteral("IDE"), defaultIde);
}

void MainWindow::applyStyle(QStyle *style)
{
    qDebug() << "Using" << style << "style";

    // note: do not set this as parent of default style
    // this will cause the style being deleted too early through ~QObject()
    // other objects (e.g. the script engine debugger) still might have a
    // reference on the style during destruction
    style->setParent(QCoreApplication::instance());

    // unfortunately, setting the style is not recursive by default, unless we have a style sheet set
    setStyleSheet(QStringLiteral("I_DONT_EXIST {}"));
    setStyle(style);
}

QWidget *MainWindow::createErrorPage(const QModelIndex &index)
{
    QLabel *page = new QLabel(this);
    page->setAlignment(Qt::AlignCenter);
    // TODO show the actual plugin error message as well as any other useful information (eg. file name) we have, once the tool model has those
    page->setText(tr("Tool %1 failed to load.").arg(index.data(ToolModelRole::ToolId).toString()));
    return page;
}

void MainWindow::quitHost()
{
    emit targetQuitRequested();
    ObjectBroker::object<ProbeControllerInterface *>()->quitHost();
}

void MainWindow::detachProbe()
{
    emit targetQuitRequested();
    ObjectBroker::object<ProbeControllerInterface *>()->detachProbe();
}

void MainWindow::configureFeedback()
{
#ifndef GAMMARAY_DISABLE_FEEDBACK
    KUserFeedback::FeedbackConfigDialog dlg;
    dlg.setFeedbackProvider(m_feedbackProvider);
    dlg.exec();
#endif
}
