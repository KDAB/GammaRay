/*
  paintanalyzerinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PAINTANALYZERINTERFACE_H
#define GAMMARAY_PAINTANALYZERINTERFACE_H

#include "gammaray_common_export.h"

#include <QDataStream>
#include <QMetaType>
#include <QObject>
#include <QPainterPath>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

namespace GammaRay {

/** Communication interface for GammaRay::PaintAnalyzer. */
class GAMMARAY_COMMON_EXPORT PaintAnalyzerInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasArgumentDetails READ hasArgumentDetails WRITE setHasArgumentDetails NOTIFY hasArgumentDetailsChanged)
    Q_PROPERTY(bool hasStackTrace READ hasStackTrace WRITE setHasStackTrace NOTIFY hasStackTraceChanged)
public:
    explicit PaintAnalyzerInterface(const QString &name, QObject *parent = nullptr);
    QString name() const;

    bool hasArgumentDetails() const;
    void setHasArgumentDetails(bool hasDetails);

    bool hasStackTrace() const;
    void setHasStackTrace(bool hasStackTrace);

Q_SIGNALS:
    void hasArgumentDetailsChanged(bool);
    void hasStackTraceChanged(bool);

private:
    QString m_name;
    bool m_hasArgumentDetails;
    bool m_hasStackTrace;
};

struct PaintAnalyzerFrameData
{
    QPainterPath clipPath;
};

GAMMARAY_COMMON_EXPORT QDataStream &operator<<(QDataStream &stream, const GammaRay::PaintAnalyzerFrameData &data);
GAMMARAY_COMMON_EXPORT QDataStream &operator>>(QDataStream &stream, GammaRay::PaintAnalyzerFrameData &data);
}

Q_DECLARE_METATYPE(GammaRay::PaintAnalyzerFrameData)
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::PaintAnalyzerInterface, "com.kdab.GammaRay.PaintAnalyzer/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_PAINTANALYZERINTERFACE_H
