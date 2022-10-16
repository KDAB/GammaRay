/*
  stacktracemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STACKTRACEMODEL_H
#define GAMMARAY_STACKTRACEMODEL_H

#include "gammaray_core_export.h"
#include "execution.h"

#include <common/modelroles.h>

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay {

/*! A table model for displaying a single stack trace. */
class GAMMARAY_CORE_EXPORT StackTraceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit StackTraceModel(QObject *parent = nullptr);
    ~StackTraceModel() override;

    void setStackTrace(const Execution::Trace &trace);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QStringList fullTrace() const;

private:
    mutable QVector<Execution::ResolvedFrame> m_frames;
    Execution::Trace m_trace;
};
}

#endif // GAMMARAY_STACKTRACEMODEL_H
