/*
  probeabimodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROBEABIMODEL_H
#define GAMMARAY_PROBEABIMODEL_H

#include <launcher/core/probeabi.h>

#include <QAbstractListModel>
#include <QVector>

namespace GammaRay {
/** Model for selecting a probe ABI in the launcher UI. */
class ProbeABIModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProbeABIModel(QObject *parent = nullptr);
    ~ProbeABIModel() override;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;

    int indexOfBestMatchingABI(const ProbeABI &targetABI) const;

private:
    QVector<ProbeABI> m_abis;
};
}

// not in its own header to not pollute the metatype space in the target
Q_DECLARE_METATYPE(GammaRay::ProbeABI)

#endif // GAMMARAY_PROBEABIMODEL_H
