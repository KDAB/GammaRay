/*
  probeabimodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_PROBEABIMODEL_H
#define GAMMARAY_PROBEABIMODEL_H

#include <common/probeabi.h>

#include <QAbstractListModel>
#include <QVector>

namespace GammaRay {

/** Model for selecting a probe ABI in the launcher UI. */
class ProbeABIModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ProbeABIModel(QObject *parent = 0);
    ~ProbeABIModel();

    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent) const;

    int indexOfBestMatchingABI(const ProbeABI &targetABI) const;

private:
    QVector<ProbeABI> m_abis;

};

}

// not in its own header to not pollute the metatype space in the target
Q_DECLARE_METATYPE(GammaRay::ProbeABI)

#endif // GAMMARAY_PROBEABIMODEL_H
