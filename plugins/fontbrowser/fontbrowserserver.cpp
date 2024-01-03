/*
  fontbrowserserver.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "fontbrowserserver.h"

#include "fontmodel.h"
#include "fontdatabasemodel.h"

#include <core/probe.h>
#include <common/objectbroker.h>

#include <QItemSelectionModel>
#include <QFontDatabase>

using namespace GammaRay;

FontBrowserServer::FontBrowserServer(Probe *probe, QObject *parent)
    : FontBrowserInterface(parent)
    , m_selectedFontModel(new FontModel(this))
{
    auto model = new FontDatabaseModel(this);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.FontModel"), model);
    m_fontSelectionModel = ObjectBroker::selectionModel(model);
    connect(m_fontSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &FontBrowserServer::updateFonts);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SelectedFontModel"),
                         m_selectedFontModel);
}

void FontBrowserServer::updateFonts()
{
    const auto rows = m_fontSelectionModel->selectedRows();
    QVector<QFont> currentFonts;
    currentFonts.reserve(rows.size());
    for (const QModelIndex &index : rows)
        currentFonts << index.data(FontRole).value<QFont>();
    m_selectedFontModel->updateFonts(currentFonts);
}

void FontBrowserServer::setPointSize(int size)
{
    m_selectedFontModel->setPointSize(size);
}

void FontBrowserServer::toggleBoldFont(bool bold)
{
    m_selectedFontModel->toggleBoldFont(bold);
}

void FontBrowserServer::toggleItalicFont(bool italic)
{
    m_selectedFontModel->toggleItalicFont(italic);
}

void FontBrowserServer::toggleUnderlineFont(bool underline)
{
    m_selectedFontModel->toggleUnderlineFont(underline);
}

void FontBrowserServer::updateText(const QString &text)
{
    m_selectedFontModel->updateText(text);
}

void FontBrowserServer::setColors(const QColor &foreground, const QColor &background)
{
    m_selectedFontModel->setColors(foreground, background);
}
