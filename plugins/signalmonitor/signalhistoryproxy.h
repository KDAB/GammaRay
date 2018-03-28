/*
  signalhistoryproxy.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#ifndef GAMMARAY_SIGNALHISTORYPROXY_H
#define GAMMARAY_SIGNALHISTORYPROXY_H

#include <core/remote/serverproxymodel.h>

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
typedef ServerProxyModel<QSortFilterProxyModel> SignalHistoryBaseProxy;

class SignalHistoryProxy : public SignalHistoryBaseProxy
{
    Q_OBJECT
public:
    explicit SignalHistoryProxy(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private slots:
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                        const QVector<int> &roles = QVector<int>());
    void updateHeaderCheckState();

private:
    Qt::CheckState m_headerMonitorCheckState;
    QTimer *m_checkStateHeaderChangedTimer;
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYPROXY_H
