/*
  remoteviewwidget.cpp

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

#include "remoteviewwidget.h"

#include <common/remoteviewinterface.h>
#include <common/objectbroker.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItemModel>

using namespace GammaRay;

RemoteViewWidget::RemoteViewWidget(QWidget* parent):
    QWidget(parent),
    m_zoomLevelModel(new QStandardItemModel(this)),
    m_unavailableText(tr("No remote view available.")),
    m_interactionModeActions(new QActionGroup(this)),
    m_interface(Q_NULLPTR),
    m_zoom(1.0),
    m_x(0),
    m_y(0),
    m_interactionMode(NoInteraction),
    m_supportedInteractionModes(ViewInteraction | Measuring | ElementPicking | InputRedirection)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setMinimumSize(QSize(400, 300));
    setFocusPolicy(Qt::StrongFocus);

    // Background
    QPixmap bgPattern(20, 20);
    bgPattern.fill(Qt::lightGray);
    QPainter bgPainter(&bgPattern);
    bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
    bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
    m_backgroundBrush.setTexture(bgPattern);

    m_zoomLevels.reserve(8);
    m_zoomLevels <<  .125 << .25 << .5 << 1.0 << 2.0 << 4.0 << 8.0 << 16.0;
    foreach (const auto level, m_zoomLevels) {
        auto item = new QStandardItem;
        item->setText(QString::number(level * 100.0) + locale().percent());
        item->setData(level, Qt::UserRole);
        m_zoomLevelModel->appendRow(item);
    }

    setupActions();
    connect(m_interactionModeActions, SIGNAL(triggered(QAction*)), this, SLOT(interactionActionTriggered(QAction*)));

    setInteractionMode(ViewInteraction);
}

RemoteViewWidget::~RemoteViewWidget()
{
}

void RemoteViewWidget::setName(const QString& name)
{
    m_interface = ObjectBroker::object<RemoteViewInterface*>(name);
    connect(m_interface, SIGNAL(reset()), this, SLOT(reset()));
}

void RemoteViewWidget::setupActions()
{
    m_interactionModeActions->setExclusive(true);

    auto action = new QAction(QIcon(QStringLiteral(":/gammaray/ui/move-preview.png")), tr("Pan View"), this);
    action->setCheckable(true);
    action->setToolTip(tr("<b>Pan view</b><br>"
        "Default mode. Click and drag to move the preview. Won't impact the original application in any way."));
    action->setData(ViewInteraction);
    action->setActionGroup(m_interactionModeActions);
    addAction(action);

    action = new QAction(QIcon(QStringLiteral(":/gammaray/ui/measure-pixels.png")), tr("Measure Pixel Sizes"), this);
    action->setCheckable(true);
    action->setToolTip(tr("<b>Measure pixel-sizes</b><br>"
        "Choose this mode, click somewhere and drag to measure the distance between the "
        "point you clicked and the point where your mouse pointer is. (Measured in scene "
        "coordinates)."));
    action->setData(Measuring);
    action->setActionGroup(m_interactionModeActions);
    addAction(action);

    // TODO: icon
    action = new QAction(tr("Pick Element"), this);
    action->setIconText(tr("Pick"));
    action->setCheckable(true);
    action->setToolTip(tr("<b>Pick Element</b><br>"
        "Select an element for inspection by clicking on it."));
    action->setData(ElementPicking);
    action->setActionGroup(m_interactionModeActions);
    addAction(action);

    action = new QAction(QIcon(QStringLiteral(":/gammaray/ui/redirect-input.png")), tr("Redirect Input"), this);
    action->setCheckable(true);
    action->setToolTip(tr("<b>Redirect Input</b><br>"
        "In this mode all mouse input is redirected directly to the original application,"
        "so you can control the application directly from within GammaRay."));
    action->setData(InputRedirection);
    action->setActionGroup(m_interactionModeActions);
    addAction(action);
}

const QImage& RemoteViewWidget::image() const
{
    return m_sourceImage;
}

void RemoteViewWidget::setImage(const QImage& image)
{
    if (m_sourceImage.isNull()) {
        m_sourceImage = image;
        fitToView();
    } else {
        m_sourceImage = image;
        update();
    }
}

void RemoteViewWidget::reset()
{
    m_sourceImage = QImage();
    update();
}

void RemoteViewWidget::setUnavailableText(const QString& msg)
{
    m_unavailableText = msg;
    update();
}

QActionGroup* RemoteViewWidget::interactionModeActions() const
{
    return m_interactionModeActions;
}

double RemoteViewWidget::zoom() const
{
    return m_zoom;
}

int RemoteViewWidget::zoomLevelIndex() const
{
    const auto it = std::lower_bound(m_zoomLevels.begin(), m_zoomLevels.end(), m_zoom);
    return std::distance(m_zoomLevels.begin(), it);
}

void RemoteViewWidget::setZoom(double zoom)
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    const auto oldZoom = m_zoom;

    // snap to nearest zoom level
    int index = 0;
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), zoom);
    if (it == m_zoomLevels.constEnd()) {
        index = m_zoomLevels.size() - 1;
    } else if (it != m_zoomLevels.constBegin()) {
        const auto delta = (*it) - zoom;
        index = std::distance(m_zoomLevels.constBegin(), it);
        --it;
        if (zoom - (*it) < delta) {
            --index;
        }
    }

    if (m_zoomLevels.at(index) == oldZoom)
        return;
    m_zoom = m_zoomLevels.at(index);
    emit zoomChanged();
    emit zoomLevelChanged(index);

    m_x = contentWidth() / 2 - (contentWidth() / 2 - m_x) * m_zoom / oldZoom;
    m_y = contentHeight() / 2 - (contentHeight() / 2 - m_y) * m_zoom / oldZoom;
    update();
}

void RemoteViewWidget::setZoomLevel(int index)
{
    Q_ASSERT(index >= 0 && index < m_zoomLevels.size());
    setZoom(m_zoomLevels.at(index));
}

void RemoteViewWidget::zoomIn()
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), m_zoom);
    if (it == m_zoomLevels.constEnd())
        return;
    ++it;
    if (it == m_zoomLevels.constEnd())
        return;
    setZoom(*it);
}

void RemoteViewWidget::zoomOut()
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), m_zoom);
    if (it == m_zoomLevels.constBegin())
        return;
    --it;
    setZoom(*it);
}

void RemoteViewWidget::fitToView()
{
    const auto scale = std::min<double>(1.0, std::min((double)contentWidth() / (double)m_sourceImage.width(), (double)contentHeight() / (double)m_sourceImage.height()));
    setZoom(scale);
    m_x = 0.5 * (contentWidth() - m_sourceImage.width() * m_zoom);
    m_y = 0.5 * (contentHeight() - m_sourceImage.height() * m_zoom);
    update();
}

QAbstractItemModel* RemoteViewWidget::zoomLevelModel() const
{
    return m_zoomLevelModel;
}

RemoteViewWidget::InteractionMode RemoteViewWidget::interactionMode() const
{
    return m_interactionMode;
}

void RemoteViewWidget::setInteractionMode(RemoteViewWidget::InteractionMode mode)
{
    if (m_interactionMode == mode || !(mode & m_supportedInteractionModes))
        return;

    setContextMenuPolicy(Qt::ActionsContextMenu);
    switch (mode) {
        case Measuring:
        case ElementPicking:
            setCursor(Qt::CrossCursor);
            break;
        case ViewInteraction:
            setCursor(Qt::OpenHandCursor);
            break;
        case InputRedirection:
            setContextMenuPolicy(Qt::PreventContextMenu);
        default:
            setCursor(QCursor());
            break;
    }

    m_interactionMode = mode;
    foreach (auto action, m_interactionModeActions->actions()) {
        if (action->data() == mode)
            action->setChecked(true);
    }
}

RemoteViewWidget::InteractionModes RemoteViewWidget::supportedInteractionModes() const
{
    return m_supportedInteractionModes;
}

void RemoteViewWidget::setSupportedInteractionModes(RemoteViewWidget::InteractionModes modes)
{
    m_supportedInteractionModes = modes;
    foreach (auto action, m_interactionModeActions->actions()) {
        action->setVisible(action->data().toInt() & modes);
    }
}

void RemoteViewWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);

    if (m_sourceImage.isNull()) {
        QWidget::paintEvent(event);
        p.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, m_unavailableText);
        return;
    }

    p.fillRect(rect(), m_backgroundBrush);

    p.save();
    p.setTransform(QTransform::fromTranslate(m_x, m_y));
    if (m_zoom < 1.0) { // We want the preview to look nice when zoomed out,
                        // but need to be able to see single pixels when zoomed in.
        p.setRenderHint(QPainter::SmoothPixmapTransform);
    }
    p.drawImage(QRect(QPoint(0, 0), m_sourceImage.size() * m_zoom), m_sourceImage);
    drawDecoration(&p);
    p.restore();

    drawRuler(&p);

    if (m_interactionMode == Measuring && (QApplication::mouseButtons() & Qt::LeftButton)) {
        drawMeasureOverlay(&p);
    }
}

void RemoteViewWidget::drawDecoration(QPainter* p)
{
    Q_UNUSED(p);
}

static int tickLength(int sourcePos, int labelDistance)
{
    int l = 8;
    if (sourcePos % labelDistance == 0)
        return 2 * l;
    if (sourcePos % 10 == 0)
        return l + 2;
    else if (sourcePos % 5 == 0)
        return l + 1;
    return l;
}

void RemoteViewWidget::drawRuler(QPainter* p)
{
    p->save();

    const int hRulerHeight = horizontalRulerHeight();
    const int vRulerWidth = verticalRulerWidth();

    const int viewTickStep = std::max<int>(2, m_zoom);
    const int viewLabelDist = viewTickLabelDistance();
    const int sourceLabelDist = sourceTickLabelDistance(viewLabelDist);

    const auto activePen = QPen(QColor(255, 255, 255, 170));
    const auto inactivePen = QPen(QColor(0, 0, 0, 170));
    const auto selectedPen = QPen(palette().color(QPalette::Highlight));

    // background
    p->setPen(Qt::NoPen);
    p->setBrush(QBrush(QColor(51, 51, 51, 170)));
    p->drawRect(QRect(0, height() - hRulerHeight, width(), hRulerHeight));
    p->drawRect(QRect(width() - vRulerWidth, 0, vRulerWidth, height() - hRulerHeight));

    // horizontal ruler at the bottom
    p->save();
    p->translate(0, height() - hRulerHeight);
    for (int i = (m_x % viewTickStep); i < contentWidth(); i += viewTickStep) {
        const int sourcePos = (i - m_x) / m_zoom;
        if (sourcePos == m_currentMousePosition.x())
            p->setPen(selectedPen);
        else if (sourcePos < 0 || sourcePos > m_sourceImage.width())
            p->setPen(inactivePen);
        else
            p->setPen(activePen);

        const int tickSize = tickLength(sourcePos, sourceLabelDist);
        p->drawLine(i, 0, i, tickSize);

        if (sourcePos >= 0 && sourcePos <= m_sourceImage.width() && sourcePos % sourceLabelDist == 0) {
            p->setPen(activePen);
            p->drawText(i - viewLabelDist / 2, tickSize, viewLabelDist, hRulerHeight - tickSize, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(sourcePos));
        }
    }
    p->restore();

    // vertical ruler on the right
    p->save();
    p->translate(width() - vRulerWidth, 0);
    for (int i = (m_y % viewTickStep); i < contentHeight(); i += viewTickStep) {
        const int sourcePos = (i - m_y) / m_zoom;
        if (sourcePos == m_currentMousePosition.y())
            p->setPen(selectedPen);
        else if (sourcePos < 0 || sourcePos > m_sourceImage.height())
            p->setPen(inactivePen);
        else
            p->setPen(activePen);

        const int tickSize = tickLength(sourcePos, sourceLabelDist);
        p->drawLine(0, i, tickSize, i);

        if (sourcePos >= 0 && sourcePos <= m_sourceImage.height() && sourcePos % sourceLabelDist == 0) {
            p->setPen(activePen);
            p->drawText(tickSize, i - viewLabelDist / 2, vRulerWidth - tickSize, viewLabelDist, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(sourcePos));
        }
    }
    p->restore();

    p->setPen(activePen);
    p->drawText(QRect(width() - vRulerWidth, height() - hRulerHeight, vRulerWidth, hRulerHeight),
                QStringLiteral("%1x\n%2").arg(m_currentMousePosition.x()).arg(m_currentMousePosition.y()),
                Qt::AlignHCenter | Qt::AlignVCenter
               );
    p->restore();
}

int RemoteViewWidget::viewTickLabelDistance() const
{
    const auto maxLabel = std::max(m_sourceImage.width(), m_sourceImage.height());
    return 2 * fontMetrics().width(QString::number(maxLabel));
}

int RemoteViewWidget::sourceTickLabelDistance(int viewDistance)
{
    Q_ASSERT(viewDistance > 0);

    if (m_tickLabelDists.isEmpty()) {
        m_tickLabelDists.reserve(11);
        m_tickLabelDists << 5 << 10 << 20 << 25 << 50 << 100 << 200 << 250 << 500 << 1000 << 2000;
    }
    const int sourceDist = viewDistance / m_zoom;
    while (sourceDist > *m_tickLabelDists.constEnd()) {
        m_tickLabelDists.push_back(m_tickLabelDists.at(m_tickLabelDists.size() - 4) * 10);
    }

    const auto it = std::lower_bound(m_tickLabelDists.constBegin(), m_tickLabelDists.constEnd(), sourceDist);
    return *it;
}

void RemoteViewWidget::drawMeasureOverlay(QPainter* p)
{
    p->save();

    p->setCompositionMode(QPainter::CompositionMode_Difference);
    p->setPen(QColor(255, 255, 255, 170));

    const auto startPos =  mapFromSource(m_mouseDownPosition);
    const auto endPos = mapFromSource(m_currentMousePosition);
    const QPoint hOffset(5, 0);
    const QPoint vOffset(0, 5);

    p->drawLine(startPos - hOffset, startPos + hOffset);
    p->drawLine(startPos - vOffset, startPos + vOffset);

    p->drawLine(startPos, endPos);

    p->drawLine(endPos - hOffset, endPos + hOffset);
    p->drawLine(endPos - vOffset, endPos + vOffset);

    p->restore();
}

QPoint RemoteViewWidget::mapToSource(QPoint pos) const
{
    return (pos - QPoint(m_x, m_y)) / m_zoom;
}

QPoint RemoteViewWidget::mapFromSource(QPoint pos) const
{
    return pos * m_zoom + QPoint(m_x, m_y);
}

void RemoteViewWidget::resizeEvent(QResizeEvent* event)
{
    m_x += 0.5 * (event->size().width() - event->oldSize().width());
    m_y += 0.5 * (event->size().height() - event->oldSize().height());

    QWidget::resizeEvent(event);
}

void RemoteViewWidget::mousePressEvent(QMouseEvent* event)
{
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case ViewInteraction:
            m_mouseDownPosition = event->pos() - QPoint(m_x, m_y);
            if ((event->modifiers() & Qt::ControlModifier) && (m_supportedInteractionModes & ElementPicking))
                m_interface->pickElementAt(mapToSource(event->pos()));
            if (event->buttons() & Qt::LeftButton)
                setCursor(Qt::ClosedHandCursor);
             break;
        case Measuring:
            m_mouseDownPosition = mapToSource(event->pos());
            break;
        case ElementPicking:
            if (event->buttons() & Qt::LeftButton)
                m_interface->pickElementAt(mapToSource(event->pos()));
            break;
        case InputRedirection:
            sendMouseEvent(event);
            break;
    }

    QWidget::mousePressEvent(event);
}

void RemoteViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case ViewInteraction:
            setCursor(Qt::OpenHandCursor);
            break;
        case InputRedirection:
            sendMouseEvent(event);
            break;
    }

    QWidget::mouseReleaseEvent(event);
}

void RemoteViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case ViewInteraction:
            if (event->buttons() != Qt::LeftButton)
                break;
            m_x = event->x() - m_mouseDownPosition.x();
            m_y = event->y() - m_mouseDownPosition.y();

            if (m_x > width() / 2) {
                m_x = width() / 2;
            } else if (m_x + m_sourceImage.width() * m_zoom < width() / 2.0) {
                m_x = width() / 2 - m_sourceImage.width() * m_zoom;
            }
            if (m_y > height() / 2) {
                m_y = height() / 2;
            } else if (m_y + m_sourceImage.height() * m_zoom < height() / 2.0) {
                m_y = height() / 2 - m_sourceImage.height() * m_zoom;
            }
            break;
        case Measuring:
            if (event->buttons() & Qt::LeftButton)
                emit measurementChanged(QRectF(m_mouseDownPosition, m_currentMousePosition));
            break;
        case InputRedirection:
            sendMouseEvent(event);
            break;
    }

  update();
}

void RemoteViewWidget::wheelEvent(QWheelEvent *event)
{
    switch (m_interactionMode) {
        case ViewInteraction:
        case ElementPicking:
        case Measuring:
            // TODO pan if Ctrl isn't pressed
            if (event->modifiers() & Qt::ControlModifier && event->orientation() == Qt::Vertical) {
                if (event->delta() > 0)
                    zoomIn();
                else
                    zoomOut();
            }
            break;
        case InputRedirection:
            sendWheelEvent(event);
            break;
    }

    QWidget::wheelEvent(event);
}

void RemoteViewWidget::keyPressEvent(QKeyEvent* event)
{
    switch (m_interactionMode) {
        case ViewInteraction:
        case ElementPicking:
        case Measuring:
            if (event->key() == Qt::Key_Plus && event->modifiers() & Qt::ControlModifier)
                zoomIn();
            if (event->key() == Qt::Key_Minus && event->modifiers() & Qt::ControlModifier)
                zoomOut();
            break;
        case InputRedirection:
            sendKeyEvent(event);
            break;
    }
    QWidget::keyPressEvent(event);
}

void RemoteViewWidget::keyReleaseEvent(QKeyEvent* event)
{
    switch (m_interactionMode) {
        case InputRedirection:
            sendKeyEvent(event);
            break;
    }
    QWidget::keyReleaseEvent(event);
}

int RemoteViewWidget::contentWidth() const
{
    return width() - verticalRulerWidth();
}

int RemoteViewWidget::contentHeight() const
{
    return height() - horizontalRulerHeight();
}

int RemoteViewWidget::verticalRulerWidth() const
{
    return fontMetrics().width(QString::number(m_sourceImage.height())) + 24; // 2* tick length + some margin
}

int RemoteViewWidget::horizontalRulerHeight() const
{
    return fontMetrics().height() + 20; // 2 * tick length + some margin
}

void RemoteViewWidget::interactionActionTriggered(QAction* action)
{
    Q_ASSERT(action);
    setInteractionMode(static_cast<InteractionMode>(action->data().toInt()));
}

void RemoteViewWidget::sendMouseEvent(QMouseEvent* event)
{
    m_interface->sendMouseEvent(event->type(), mapToSource(event->pos()), event->button(), event->buttons(), event->modifiers());
}

void RemoteViewWidget::sendKeyEvent(QKeyEvent* event)
{
    m_interface->sendKeyEvent(event->type(), event->key(), event->modifiers(), event->text(), event->isAutoRepeat(), event->count());
}

void RemoteViewWidget::sendWheelEvent(QWheelEvent* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    auto angleDelta = event->angleDelta();
    auto pixelDelta = event->pixelDelta();
#else
    QPoint angleDelta;
    if (event->orientation() == Qt::Horizontal)
        angleDelta.setX(event->delta());
    else
        angleDelta.setY(event->delta());
    QPoint pixelDelta;
#endif
    m_interface->sendWheelEvent(mapToSource(event->pos()), pixelDelta, angleDelta, event->buttons(), event->modifiers());
}
