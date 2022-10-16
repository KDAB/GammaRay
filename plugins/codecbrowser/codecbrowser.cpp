/*
  codecbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
                             "com.kdab.GammaRay.SelectedCodecsModel"),
                         m_selectedCodecsModel);
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
