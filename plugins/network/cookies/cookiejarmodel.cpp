/*
  cookiejarmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "cookiejarmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QNetworkCookieJar>

using namespace GammaRay;

// protected member access control violation helper
class CookieMonster : public QNetworkCookieJar
{
public:
    static QList<QNetworkCookie> pillageJar(QNetworkCookieJar *jar);
};

QList<QNetworkCookie> CookieMonster::pillageJar(QNetworkCookieJar *jar)
{
    auto openJar = reinterpret_cast<CookieMonster *>(jar);
    return openJar->allCookies();
}

CookieJarModel::CookieJarModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_cookieJar(nullptr)
{
}

CookieJarModel::~CookieJarModel() = default;

void CookieJarModel::setCookieJar(QNetworkCookieJar *cookieJar)
{
    if (m_cookieJar == cookieJar)
        return;

    beginResetModel();
    m_cookieJar = cookieJar;
    if (cookieJar)
        m_cookies = CookieMonster::pillageJar(cookieJar);
    else
        m_cookies.clear();
    endResetModel();
}

int CookieJarModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

int CookieJarModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_cookieJar)
        return 0;
    return m_cookies.size();
}

QVariant CookieJarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_cookieJar)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &cookie = m_cookies.at(index.row());
        switch (index.column()) {
        case 0:
            return cookie.name();
        case 1:
            return cookie.domain();
        case 2:
            return cookie.path();
        case 3:
            return cookie.value();
        case 4:
            return cookie.expirationDate();
        }
    } else if (role == Qt::CheckStateRole) {
        const auto &cookie = m_cookies.at(index.row());
        switch (index.column()) {
        case 5:
            return cookie.isHttpOnly() ? Qt::Checked : Qt::Unchecked;
        case 6:
            return cookie.isSecure() ? Qt::Checked : Qt::Unchecked;
        case 7:
            return cookie.isSessionCookie() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant CookieJarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Domain");
        case 2:
            return tr("Path");
        case 3:
            return tr("Value");
        case 4:
            return tr("Expiration Date");
        case 5:
            return tr("Http Only");
        case 6:
            return tr("Secure");
        case 7:
            return tr("Session Cookie");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}
