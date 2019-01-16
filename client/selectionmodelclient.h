/*
  selectionmodelclient.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SELECTIONMODELCLIENT_H
#define GAMMARAY_SELECTIONMODELCLIENT_H

#include <common/networkselectionmodel.h>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
/** Client side of the network transparent QItemSelectionModel. */
class SelectionModelClient : public NetworkSelectionModel
{
    Q_OBJECT
public:
    SelectionModelClient(const QString &objectName, QAbstractItemModel *model, QObject *parent);
    ~SelectionModelClient() override;

private slots:
    void timeout();
    void serverRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
    void serverUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

private:
    void connectToServer();

    QTimer *m_timer;
};
}

#endif // GAMMARAY_SELECTIONMODELCLIENT_H
