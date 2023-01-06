/*
  toolmanagerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TOOLMANAGERINTERFACE_H
#define GAMMARAY_TOOLMANAGERINTERFACE_H

#include "objectid.h"

#include <QObject>
#include <QDataStream>
#include <QDebug>
#include <QMetaType>
#include <QVector>

namespace GammaRay {
/** @brief GammaRay tool identifier. */
struct ToolData
{
    QString id;
    bool hasUi;
    bool enabled;
};

/** @brief Probe and host process remote control functions. */
class ToolManagerInterface : public QObject
{
    Q_OBJECT

public:
    explicit ToolManagerInterface(QObject *parent = nullptr);
    ~ToolManagerInterface() override;

    virtual void selectObject(const ObjectId &id, const QString &toolId) = 0;
    virtual void requestToolsForObject(const ObjectId &id) = 0;
    virtual void requestAvailableTools() = 0;

Q_SIGNALS:
    void toolsForObjectResponse(const GammaRay::ObjectId &id, const QVector<QString> &toolInfos);
    void availableToolsResponse(const QVector<GammaRay::ToolData> &toolInfos);
    void toolEnabled(const QString &toolId);
    void toolSelected(const QString &toolId);

private:
    Q_DISABLE_COPY(ToolManagerInterface)
};

inline QDataStream &operator<<(QDataStream &out, const ToolData &toolInfo)
{
    out << toolInfo.id;
    out << toolInfo.hasUi;
    out << toolInfo.enabled;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ToolData &toolInfo)
{
    in >> toolInfo.id;
    in >> toolInfo.hasUi;
    in >> toolInfo.enabled;
    return in;
}
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::ToolManagerInterface, "com.kdab.GammaRay.ToolManagerInterface")
QT_END_NAMESPACE
Q_DECLARE_METATYPE(GammaRay::ToolData)
Q_DECLARE_METATYPE(QVector<GammaRay::ToolData>)

#endif // GAMMARAY_TOOLMANAGERINTERFACE_H
