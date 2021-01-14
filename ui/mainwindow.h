/*
  mainwindow.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_MAINWINDOW_H
#define GAMMARAY_MAINWINDOW_H

#include <QMainWindow>

#include <ui/uistatemanager.h>

QT_BEGIN_NAMESPACE
class QModelIndex;
class QSplitter;
class QHeaderView;
class QUrl;
QT_END_NAMESPACE

namespace KUserFeedback {
class Provider;
}

namespace GammaRay {
namespace Ui {
class MainWindow;
}

class ClientToolFilterProxyModel;

class MainWindowUIStateManager : public UIStateManager
{
    Q_OBJECT

public:
    explicit MainWindowUIStateManager(QWidget *widget);

    QList<QSplitter *> splitters() const override;
    QList<QHeaderView *> headers() const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void setupFeedbackProvider();

    Q_INVOKABLE void saveTargetState(QSettings *settings) const;
    Q_INVOKABLE void restoreTargetState(QSettings *settings);

signals:
    void targetQuitRequested();

private slots:
    void help();
    void configureFeedback();
    void about();
    void aboutPlugins();
    void aboutKDAB();

    void showMessageStatistics();

    void toolSelected();
    bool selectTool(const QString &id);
    void toolContextMenu(QPoint pos);

    void quitHost();
    void detachProbe();
    void navigateToCode(const QUrl &url, int lineNumber, int columnNumber);
    void logTransmissionRate(quint64 bytesRead, quint64 bytesWritten);
    void setCodeNavigationIDE(QAction *action);

private:
    QWidget *createErrorPage(const QModelIndex &index);

    /// apply custom style for GammaRay's main window
    void applyStyle(QStyle* style);

    QScopedPointer<Ui::MainWindow> ui;
    MainWindowUIStateManager m_stateManager;
    ClientToolFilterProxyModel *m_toolFilterModel;

    KUserFeedback::Provider *m_feedbackProvider;
};
}

#endif // MAINWINDOW_H
