/*
  protocol.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "protocol.h"

namespace GammaRay {
namespace Protocol {
Protocol::ModelIndex fromQModelIndex(const QModelIndex &index)
{
    if (!index.isValid())
        return ModelIndex();
    ModelIndex result = fromQModelIndex(index.parent());
    result.push_back(ModelIndexData(index.row(), index.column()));
    return result;
}

QModelIndex toQModelIndex(const QAbstractItemModel *model, const Protocol::ModelIndex &index)
{
    QModelIndex qmi;

    for (auto it = index.constBegin(), end = index.constEnd(); it != end; ++it) {
        qmi = model->index(it->row, it->column, qmi);
        if (!qmi.isValid())
            return {}; // model isn't loaded to the full depth, so don't restart from the top
    }

    return qmi;
}

qint32 version()
{
    return 38;
}

qint32 broadcastFormatVersion()
{
    return 2;
}
}
}
