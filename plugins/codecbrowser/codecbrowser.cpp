/*
  codecbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "codecbrowser.h"
#include "codecmodel.h"

#include <common/objectbroker.h>

#include <QItemSelectionModel>

#include <QtPlugin>

using namespace GammaRay;

CodecBrowser::CodecBrowser(Probe *probe, QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject(QStringLiteral("com.kdab.GammaRay.CodecBrowser"), this);

    auto *model = new AllCodecsModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.AllCodecsModel"), model);

    m_codecSelectionModel = ObjectBroker::selectionModel(model);
    connect(m_codecSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &CodecBrowser::updateCodecs);

    m_selectedCodecsModel = new SelectedCodecsModel(this);
    probe->registerModel(QStringLiteral(
                             "com.kdab.GammaRay.SelectedCodecsModel"), m_selectedCodecsModel);
}

void CodecBrowser::textChanged(const QString &text)
{
    m_selectedCodecsModel->updateText(text);
}

void CodecBrowser::updateCodecs(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    const auto rows = m_codecSelectionModel->selectedRows();
    QStringList currentCodecNames;
    currentCodecNames.reserve(rows.size());
    for (const QModelIndex &index : rows) {
        const QString codecName = index.data().toString();
        currentCodecNames.append(codecName);
    }

    m_selectedCodecsModel->setCodecs(currentCodecNames);
}
