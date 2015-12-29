/*
  paintanalyzer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "paintanalyzer.h"
#include "paintbuffermodel.h"

#include <core/probe.h>
#include <core/remoteviewserver.h>
#include <common/objectbroker.h>

#include <QItemSelectionModel>

using namespace GammaRay;

PaintAnalyzer::PaintAnalyzer(const QString& name, QObject* parent):
    PaintAnalyzerInterface(name, parent),
    m_paintBufferModel(Q_NULLPTR),
    m_paintBuffer(Q_NULLPTR),
    m_remoteView(new RemoteViewServer(name + QStringLiteral(".remoteView"), this))
{
#ifdef HAVE_PRIVATE_QT_HEADERS
    m_paintBufferModel = new PaintBufferModel(this);
    Probe::instance()->registerModel(name + QStringLiteral(".paintBufferModel"), m_paintBufferModel);
    connect(ObjectBroker::selectionModel(m_paintBufferModel), SIGNAL(currentChanged(QModelIndex,QModelIndex)), m_remoteView, SLOT(sourceChanged()));
#endif

    connect(m_remoteView, SIGNAL(requestUpdate()), this, SLOT(repaint()));
}

PaintAnalyzer::~PaintAnalyzer()
{
}

void PaintAnalyzer::repaint()
{
    if (!m_remoteView->isActive())
        return;

#ifdef HAVE_PRIVATE_QT_HEADERS
    const QSize sourceSize = m_paintBufferModel->buffer().boundingRect().size().toSize();
    QImage image(sourceSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    const auto start = m_paintBufferModel->buffer().frameStartIndex(0);

    // include selected row or paint all if nothing is selected
    const auto index = ObjectBroker::selectionModel(m_paintBufferModel)->currentIndex();
    const auto end = index.isValid() ? index.row() + 1 : m_paintBufferModel->rowCount();
    auto depth = m_paintBufferModel->buffer().processCommands(&painter, start, start + end);
    for (; depth > 0; --depth)
        painter.restore();
    painter.end();
    emit paintingAnalyzed(image);
#endif
}

void PaintAnalyzer::beginAnalyzePainting()
{
    Q_ASSERT(!m_paintBuffer);
#ifdef HAVE_PRIVATE_QT_HEADERS
    m_paintBuffer = new QPaintBuffer;
#endif
}

void PaintAnalyzer::setBoundingRect(const QRectF& boundingBox)
{
#ifdef HAVE_PRIVATE_QT_HEADERS
    Q_ASSERT(m_paintBuffer);
    m_paintBuffer->setBoundingRect(boundingBox);
#else
    Q_UNUSED(boundingBox);
#endif
}

QPaintDevice* PaintAnalyzer::paintDevice() const
{
#ifdef HAVE_PRIVATE_QT_HEADERS
    Q_ASSERT(m_paintBuffer);
    return m_paintBuffer;
#else
    return Q_NULLPTR;
#endif
}

void PaintAnalyzer::endAnalyzePainting()
{
#ifdef HAVE_PRIVATE_QT_HEADERS
    Q_ASSERT(m_paintBuffer);
    Q_ASSERT(m_paintBufferModel);
    m_paintBufferModel->setPaintBuffer(*m_paintBuffer);
#endif
    delete m_paintBuffer;
    m_paintBuffer = 0;
    m_remoteView->resetView();
    m_remoteView->sourceChanged();
}

bool PaintAnalyzer::isAvailable()
{
#ifdef HAVE_PRIVATE_QT_HEADERS
    return true;
#else
    return false;
#endif
}
