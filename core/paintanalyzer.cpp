/*
  paintanalyzer.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "paintanalyzer.h"
#include "paintbuffer.h"
#include "paintbuffermodel.h"
#include "painterprofilingreplayer.h"

#include <core/aggregatedpropertymodel.h>
#include <core/probe.h>
#include <core/remoteviewserver.h>
#include <core/stacktracemodel.h>
#include <core/remote/serverproxymodel.h>

#include <common/metatypedeclarations.h>
#include <common/objectbroker.h>
#include <common/remoteviewframe.h>
#include <common/paintbuffermodelroles.h>

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

using namespace GammaRay;

class PaintBufferModelFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PaintBufferModelFilterProxy(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }
    void sort(int, Qt::SortOrder) override
    {
    } // never sort, that has no semantics here
};

PaintAnalyzer::PaintAnalyzer(const QString &name, QObject *parent)
    : PaintAnalyzerInterface(name, parent)
    , m_paintBufferModel(nullptr)
    , m_paintBufferFilter(nullptr)
    , m_selectionModel(nullptr)
    , m_paintBuffer(nullptr)
    , m_remoteView(new RemoteViewServer(name + QStringLiteral(".remoteView"), this))
    , m_argumentModel(new AggregatedPropertyModel(this))
    , m_stackTraceModel(new StackTraceModel(this))
{
    m_paintBufferModel = new PaintBufferModel(this);
    auto proxy = new ServerProxyModel<PaintBufferModelFilterProxy>(this);
    proxy->addRole(PaintBufferModelRoles::MaxCostRole);
    m_paintBufferFilter = proxy;
    m_paintBufferFilter->setSourceModel(m_paintBufferModel);
    Probe::instance()->registerModel(name + QStringLiteral(".paintBufferModel"), m_paintBufferFilter);

    m_selectionModel = ObjectBroker::selectionModel(m_paintBufferFilter);
    connect(m_selectionModel, &QItemSelectionModel::currentChanged, m_remoteView,
            &RemoteViewServer::sourceChanged);

    m_argumentModel->setReadOnly(true);
    Probe::instance()->registerModel(name + QStringLiteral(".argumentProperties"), m_argumentModel);
    Probe::instance()->registerModel(name + QStringLiteral(".stackTrace"), m_stackTraceModel);

    connect(m_remoteView, &RemoteViewServer::requestUpdate, this, &PaintAnalyzer::repaint);
}

PaintAnalyzer::~PaintAnalyzer() = default;

void PaintAnalyzer::reset()
{
    m_remoteView->sourceChanged();
    m_paintBufferModel->setPaintBuffer(PaintBuffer());
}

void PaintAnalyzer::repaint()
{
    if (!m_remoteView->isActive())
        return;

    if (m_paintBufferModel->rowCount() == 0) {
        emit requestUpdate();
        return;
    }

    const QSize sourceSize = m_paintBufferModel->buffer().boundingRect().size().toSize();
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    const qreal ratio = m_paintBufferModel->buffer().devicePixelRatioF();
#else
    const qreal ratio = m_paintBufferModel->buffer().devicePixelRatio();
#endif
    QImage image(sourceSize * ratio, QImage::Format_ARGB32);
    image.setDevicePixelRatio(ratio);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    const auto start = m_paintBufferModel->buffer().frameStartIndex(0);

    auto index = m_paintBufferFilter->mapToSource(m_selectionModel->currentIndex());
    m_currentArgument = index.data(PaintBufferModelRoles::ValueRole);
    m_argumentModel->setObject(m_currentArgument);
    setHasArgumentDetails(m_argumentModel->rowCount());

    // include selected row or paint all if nothing is selected
    if (index.parent().isValid()) {
        index = index.parent();
    }
    const auto end = index.isValid() ? index.row() + 1 : m_paintBufferModel->rowCount();
    auto depth = m_paintBufferModel->buffer().processCommands(&painter, start, start + end);
    for (; depth > 0; --depth) {
        painter.restore();
    }
    painter.end();

    PaintAnalyzerFrameData data;
    if (index.isValid()) {
        data.clipPath = index.data(PaintBufferModelRoles::ClipPathRole).value<QPainterPath>();
    }
    RemoteViewFrame frame;
    frame.setImage(image);
    frame.data = QVariant::fromValue(data);
    m_remoteView->sendFrame(frame);

    if (index.isValid()) {
        m_stackTraceModel->setStackTrace(m_paintBufferModel->buffer().stackTrace(index.row()));
        setHasStackTrace(m_stackTraceModel->rowCount() > 0);
    } else {
        setHasStackTrace(false);
    }
}

void PaintAnalyzer::beginAnalyzePainting()
{
    Q_ASSERT(!m_paintBuffer);
    m_paintBuffer = new PaintBuffer;
}

void PaintAnalyzer::setBoundingRect(const QRectF &boundingBox)
{
    Q_ASSERT(m_paintBuffer);
    m_paintBuffer->setBoundingRect(boundingBox);
}

QPaintDevice *PaintAnalyzer::paintDevice() const
{
    Q_ASSERT(m_paintBuffer);
    return m_paintBuffer;
}

void PaintAnalyzer::endAnalyzePainting()
{
    Q_ASSERT(m_paintBuffer);
    Q_ASSERT(m_paintBufferModel);
    m_paintBufferModel->setPaintBuffer(*m_paintBuffer);
    delete m_paintBuffer;
    m_paintBuffer = nullptr;
    m_remoteView->resetView();
    m_remoteView->sourceChanged();

    if (auto rowCount = m_paintBufferFilter->rowCount()) {
        const auto idx = m_paintBufferFilter->index(rowCount - 1, 0);
        m_selectionModel->select(idx,
                                 QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Current);
    }

    PainterProfilingReplayer profiler;
    profiler.profile(m_paintBufferModel->buffer());
    m_paintBufferModel->setCosts(profiler.costs());
}

void GammaRay::PaintAnalyzer::setOrigin(const ObjectId &obj)
{
    m_paintBuffer->setOrigin(obj);
}

bool PaintAnalyzer::isAvailable()
{
    return true;
}

#include "paintanalyzer.moc"
