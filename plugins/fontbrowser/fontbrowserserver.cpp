/*
  fontbrowserserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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
