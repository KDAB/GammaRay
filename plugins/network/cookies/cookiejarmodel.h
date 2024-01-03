/*
  cookiejarmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_COOKIEJARMODEL_H
#define GAMMARAY_COOKIEJARMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QNetworkCookie>

QT_BEGIN_NAMESPACE
class QNetworkCookieJar;
QT_END_NAMESPACE

namespace GammaRay {
class CookieJarModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CookieJarModel(QObject *parent = nullptr);
    ~CookieJarModel() override;

    void setCookieJar(QNetworkCookieJar *cookieJar);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QNetworkCookieJar *m_cookieJar;
    QList<QNetworkCookie> m_cookies;
};
}

#endif // GAMMARAY_COOKIEJARMODEL_H
