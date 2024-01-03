/*
  networkconfigurationmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKCONFIGURATIONMODEL_H
#define GAMMARAY_NETWORKCONFIGURATIONMODEL_H

#include "networkconfigurationmodelroles.h"

#include <QAbstractTableModel>
#include <QNetworkConfiguration>

#include <vector>

QT_BEGIN_NAMESPACE
class QNetworkConfigurationManager;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QNetworkConfiguration::Purpose)
Q_DECLARE_METATYPE(QNetworkConfiguration::StateFlags)
Q_DECLARE_METATYPE(QNetworkConfiguration::Type)

namespace GammaRay {

class NetworkConfigurationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NetworkConfigurationModel(QObject *parent = nullptr);
    ~NetworkConfigurationModel() override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private slots:
    void init();

private:
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);

    QNetworkConfigurationManager *m_mgr;
    std::vector<QNetworkConfiguration> m_configs;
};
}

#endif // GAMMARAY_NETWORKCONFIGURATIONMODEL_H
