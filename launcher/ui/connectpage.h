/*
  connectpage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CONNECTPAGE_H
#define GAMMARAY_CONNECTPAGE_H

#include <QWidget>
#include <QHostAddress>
#include <QToolButton>
#include <QUrl>

namespace GammaRay {
namespace Ui {
class ConnectPage;
}

/** UI for connecting to a running GammaRay instance. */
class ConnectPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectPage(QWidget *parent = nullptr);
    ~ConnectPage();

    bool isValid() const;
    void writeSettings();

public slots:
    void launchClient();

signals:
    void updateButtonState();
    void activate();

private slots:
    void instanceSelected();
    void validateHostAddress(const QString &address);

private:
    void handleLocalAddress(QString &stillToParse, bool &correctSoFar);
    void handleIPAddress(QString &stillToParse, bool &correctSoFar);
    void handleAddressAndPort(QString &stillToParse, bool &correctSoFar, const QString &possibleAddress, bool skipPort = false);
    void handlePortString(QString &stillToParse, bool &correctSoFar);

    void showStandardPortAssumedWarning();

    QScopedPointer<Ui::ConnectPage> ui;
    QUrl m_currentUrl;
    bool m_valid;
    QToolButton *m_portInfoLabel;
    const QLatin1String m_localPrefix;
    const QLatin1String m_tcpPrefix;
    QAction *m_implicitPortWarningSign;
    QAction *m_fileIsNotASocketWarning;
};
}

#endif // GAMMARAY_CONNECTPAGE_H
