/*
  stacktracemodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

private:
    mutable QVector<Execution::ResolvedFrame> m_frames;
    Execution::Trace m_trace;
};
}

#endif // GAMMARAY_STACKTRACEMODEL_H
