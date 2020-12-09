/*
  modelinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_MODELINSPECTORINTERFACE_H
#define GAMMARAY_MODELINSPECTORINTERFACE_H

#include <QObject>
#include <QMetaType>

namespace GammaRay {

struct ModelCellData
{
    ModelCellData() = default;
    bool operator==(const ModelCellData &other) const;

    int row = -1;
    int column = -1;
    QString internalId;
    QString internalPtr;
    Qt::ItemFlags flags = Qt::NoItemFlags;
};

QDataStream &operator<<(QDataStream &out, const ModelCellData &data);
QDataStream &operator>>(QDataStream &in, ModelCellData &data);

class ModelInspectorInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(GammaRay::ModelCellData cellData READ currentCellData WRITE setCurrentCellData NOTIFY currentCellDataChanged)
public:
    explicit ModelInspectorInterface(QObject *parent = nullptr);
    ~ModelInspectorInterface() override;

    ModelCellData currentCellData() const;
    void setCurrentCellData(const ModelCellData &cellData);

signals:
    void currentCellDataChanged();

private:
    ModelCellData m_currentCellData;
};
}

Q_DECLARE_METATYPE(GammaRay::ModelCellData)
QT_BEGIN_NAMESPACE
Q_DECLARE_TYPEINFO(GammaRay::ModelCellData, Q_MOVABLE_TYPE);
Q_DECLARE_INTERFACE(GammaRay::ModelInspectorInterface, "com.kdab.GammaRay.ModelInspectorInterface")
QT_END_NAMESPACE

#endif // GAMMARAY_MODELINSPECTORINTERFACE_H
