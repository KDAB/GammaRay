/*
  cookiejarmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
