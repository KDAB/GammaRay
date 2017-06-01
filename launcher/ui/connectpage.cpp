/*
  connectpage.cpp

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

#include "connectpage.h"
#include "ui_connectpage.h"

#include <launcher/core/networkdiscoverymodel.h>
#include <launcher/core/clientlauncher.h>

#include <common/endpoint.h>

#include <QAction>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>

#include <qplatformdefs.h>

#ifdef Q_OS_LINUX
#include <sys/stat.h>
#endif

using namespace GammaRay;

ConnectPage::ConnectPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectPage)
    , m_localPrefix("local://")
    , m_tcpPrefix("tcp://")
{
    ui->setupUi(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_implicitPortWarningSign = new QAction(QIcon(":/launcher/warning.png"), "No port given, will use 11732", this);
    m_fileIsNotASocketWarning = new QAction(qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical)
                                            , "File is not a socket"
                                            , this);
#endif
    connect(ui->host, SIGNAL(textChanged(QString)), SLOT(validateHostAddress(const QString&)));
    connect(ui->host, SIGNAL(textChanged(QString)), SIGNAL(updateButtonState()));

    auto *model = new NetworkDiscoveryModel(this);
    ui->instanceView->setModel(model);
    connect(ui->instanceView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(instanceSelected()));
    connect(ui->instanceView, SIGNAL(activated(QModelIndex)), SIGNAL(activate()));

    QSettings settings;
    ui->host->setText(settings.value(QStringLiteral("Connect/Url"), QString()).toString());
}

void ConnectPage::validateHostAddress(const QString &address)
{
    QString stillToParse = address;
    m_valid = false;
    bool correctSoFar = false;
    m_currentUrl.clear();

    // Initially, set the text to red
    QPalette palette;
    palette.setColor(QPalette::Text,Qt::red);
    ui->host->setPalette(palette);
    clearWarnings();

    handleLocalAddress(stillToParse, correctSoFar);
    handleIPAddress(stillToParse, correctSoFar);

    // if we came down here and theres nothing more to parse, we are good
    // set text back to black again
    if (correctSoFar && stillToParse.isEmpty()) {
        m_valid = true;
        palette.setColor(QPalette::Text,Qt::black);
        ui->host->setPalette(palette);
    }
    emit userInputParsed();
}

void ConnectPage::handleLocalAddress(QString &stillToParse, bool &correctSoFar)
{
#ifdef Q_OS_UNIX
    if (stillToParse.startsWith(m_localPrefix))
        stillToParse.remove(0, m_localPrefix.size()-1); //dont remove second slash

    // Its also okay, if only a path to an existing file is given
    QFileInfo localSocketFile(stillToParse);
    if (localSocketFile.exists() && !localSocketFile.isDir() && !localSocketFile.isSymLink()) {
        QT_STATBUF statbuf;
        QT_STAT(QFile::encodeName(localSocketFile.filePath()), &statbuf);
        if(!S_ISSOCK(statbuf.st_mode)) {
            showFileIsNotSocketWarning();
        } else {
            stillToParse = "";
            correctSoFar = true;
            m_currentUrl.setScheme("local");
            m_currentUrl.setPath(localSocketFile.filePath());
        }
    }
#endif
}

void ConnectPage::handleIPAddress(QString &stillToParse, bool &correctSoFar)
{
    // handle tcp prefix
    if (stillToParse.startsWith(m_tcpPrefix))
        stillToParse.remove(0, m_tcpPrefix.size());

    // Speculate on the address format
    const auto possibleIPv4Address = QHostAddress(stillToParse.split(":").first());

    QHostAddress possibleIPv6Address;
    if (!stillToParse.contains("%"))
        possibleIPv6Address = QHostAddress(stillToParse);

    QHostAddress possibleIPv6BracketAddress;
    QRegExp bracketFormat("^\\[([0-9,a-f,\\:,\\.]*)\\].*$");
    if (bracketFormat.exactMatch(stillToParse))
        possibleIPv6BracketAddress = QHostAddress(bracketFormat.cap(1));

    QHostAddress possibleIPv6InterfaceAddress;
    QRegExp interfaceFormat("^([^\\%]*)(\\%[^\\:]+)(:[0-9]+)?$");
    if (interfaceFormat.exactMatch(stillToParse))
        possibleIPv6InterfaceAddress = QHostAddress(interfaceFormat.cap(1));

    const auto skipPort = true;
    if (!possibleIPv4Address.isNull())
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv4Address.toString());
    if (!possibleIPv6Address.isNull())
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv6Address.toString(), skipPort);
    if (!possibleIPv6BracketAddress.isNull())
        handleAddressAndPort(stillToParse, correctSoFar, "[" + possibleIPv6BracketAddress.toString() + "]");
    if (!possibleIPv6InterfaceAddress.isNull()){
        stillToParse.replace(interfaceFormat.cap(2), "");
        handleAddressAndPort(stillToParse, correctSoFar, possibleIPv6InterfaceAddress.toString());
    }
}

void ConnectPage::handleAddressAndPort(QString &stillToParse, bool &correctSoFar, const QString &possibleAddress, bool skipPort)
{
    stillToParse.replace(possibleAddress, "");
    if (stillToParse.isEmpty()) {
        correctSoFar = true;
        m_currentUrl.setScheme("tcp");
        m_currentUrl.setHost(possibleAddress);
        m_currentUrl.setPort(Endpoint::defaultPort());
        showStandardPortAssumedWarning();
    } else if(!skipPort) {
        m_currentUrl.setScheme("tcp");
        m_currentUrl.setHost(possibleAddress);
        handlePortString(stillToParse, correctSoFar);
    }
}

void ConnectPage::handlePortString(QString &stillToParse, bool &correctSoFar)
{
    QRegExp r("\\:[0-9]{1,5}");
    if (r.exactMatch(stillToParse)) {
        auto portString = r.cap(0);
        stillToParse = stillToParse.replace(portString, "");
        auto portNumber = portString.replace(":","").toInt();
        if (portNumber <= 65535){
            m_currentUrl.setPort(portNumber);
            correctSoFar = true;
        }
    }
}

void ConnectPage::showStandardPortAssumedWarning()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui->host->addAction(m_implicitPortWarningSign, QLineEdit::TrailingPosition);
#endif
}

void ConnectPage::showFileIsNotSocketWarning()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui->host->addAction(m_fileIsNotASocketWarning, QLineEdit::TrailingPosition);
#endif
}

void ConnectPage::clearWarnings()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui->host->removeAction(m_implicitPortWarningSign);
    ui->host->removeAction(m_fileIsNotASocketWarning);
#endif
}

ConnectPage::~ConnectPage()
{
}

bool ConnectPage::isValid() const
{
    return m_valid;
}

void ConnectPage::launchClient()
{
    ClientLauncher::launchDetached(m_currentUrl);
}

void ConnectPage::writeSettings()
{
    QSettings settings;
    settings.setValue(QStringLiteral("Connect/Url"), ui->host->text());
}

void ConnectPage::instanceSelected()
{
    const QModelIndexList rows = ui->instanceView->selectionModel()->selectedRows();
    if (rows.size() != 1)
        return;

    ui->host->setText(rows.first().data(NetworkDiscoveryModel::UrlStringRole).toString());
}
