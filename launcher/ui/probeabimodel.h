/*
  probeabimodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
