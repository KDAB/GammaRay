/*
  protocol.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    return 36;
}

qint32 broadcastFormatVersion()
{
    return 2;
}
}
}
