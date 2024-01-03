/*
  modelinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
