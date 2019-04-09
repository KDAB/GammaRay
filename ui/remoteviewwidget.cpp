/*
  remoteviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "modelpickerdialog.h"
#include "trailingcolorlabel.h"
#include <visibilityfilterproxymodel.h>

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/objectidfilterproxymodel.h>
#include <common/objectmodel.h>
#include <common/remoteviewinterface.h>
#include <common/streamoperators.h>

#include <compat/qasconst.h>

#include <ui/uiresources.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItemModel>

#include <cmath>
#include <cstdlib>

using namespace GammaRay;
static const qint32 RemoteViewWidgetStateVersion = 1;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::RemoteViewWidget::InteractionMode)
QT_END_NAMESPACE

RemoteViewWidget::RemoteViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_zoomLevelModel(new QStandardItemModel(this))
    , m_unavailableText(tr("No remote view available."))
    , m_interactionModeActions(new QActionGroup(this))
    , m_trailingColorLabel(new TrailingColorLabel(this))
    , m_zoom(1.0)
    , m_x(0)
    , m_y(0)
    , m_interactionMode(NoInteraction)
    , m_supportedInteractionModes(NoInteraction)
    , m_hasMeasurement(false)
    , m_pickProxyModel(new ObjectIdsFilterProxyModel(this))
    , m_invisibleItemsProxyModel(new VisibilityFilterProxyModel(this))
    , m_initialZoomDone(false)
    , m_extraViewportUpdateNeeded(true)
    , m_showFps(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setMinimumSize(QSize(400, 300));
    setFocusPolicy(Qt::StrongFocus);
    window()->setAttribute(Qt::WA_AcceptTouchEvents);
    window()->setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);

    // Background textures
    {
        QPixmap bgPattern(20, 20);
        bgPattern.fill(Qt::lightGray);
        QPainter bgPainter(&bgPattern);
        bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
        bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
        m_activeBackgroundBrush.setTexture(bgPattern);
    }
    {
        QPixmap bgPattern(20, 20);
        bgPattern.fill(Qt::darkGray);
        QPainter bgPainter(&bgPattern);
        bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
        bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
        m_inactiveBackgroundBrush.setTexture(bgPattern);
    }

    m_zoomLevels.reserve(8);
    m_zoomLevels <<  .1 << .25 << .5 << 1.0 << 2.0 << 4.0 << 8.0 << 16.0;
    for (const auto level : qAsConst(m_zoomLevels)) {
        auto item = new QStandardItem;
        item->setText(QString::number(level * 100.0) + locale().percent());
        item->setData(level, Qt::UserRole);
        m_zoomLevelModel->appendRow(item);
    }

    setupActions();
    connect(m_interactionModeActions, &QActionGroup::triggered, this,
            &RemoteViewWidget::interactionActionTriggered);

    setSupportedInteractionModes(ViewInteraction | Measuring | ElementPicking | InputRedirection | ColorPicking);
    setInteractionMode(ViewInteraction);

    window()->installEventFilter(this);
}

RemoteViewWidget::~RemoteViewWidget()
{
    window()->removeEventFilter(this);
}

void RemoteViewWidget::setName(const QString &name)
{
    m_interface = ObjectBroker::object<RemoteViewInterface *>(name);
    connect(m_interface.data(), &RemoteViewInterface::reset,
            this, &RemoteViewWidget::reset);
    connect(m_interface.data(), &RemoteViewInterface::elementsAtReceived,
            this, &RemoteViewWidget::elementsAtReceived);
    connect(m_interface.data(), &RemoteViewInterface::frameUpdated,
            this, &RemoteViewWidget::frameUpdated);
    if (isVisible()) {
        m_interface->setViewActive(true);
    }
    m_interface->clientViewUpdated();
}

void RemoteViewWidget::setupActions()
{
    m_interactionModeActions->setExclusive(true);

    auto action = new QAction(UIResources::themedIcon(QLatin1String("move-preview.png")), tr(
                                  "Pan View"), this);
    action->setObjectName("aPanView");
    action->setCheckable(true);
    action->setToolTip(tr("<b>Pan view</b><br>"
                          "Default mode. Click and drag to move the preview. Won't impact the original application in any way."));
    action->setData(ViewInteraction);
    action->setActionGroup(m_interactionModeActions);

    action = new QAction(UIResources::themedIcon(QLatin1String("measure-pixels.png")),
                         tr("Measure Pixel Sizes"), this);
    action->setObjectName("aMeasurePixelSizes");
    action->setCheckable(true);
    action->setToolTip(tr("<b>Measure pixel-sizes</b><br>"
                          "Choose this mode, click somewhere and drag to measure the distance between the "
                          "point you clicked and the point where your mouse pointer is. (Measured in scene "
                          "coordinates)."));
    action->setData(Measuring);
    action->setActionGroup(m_interactionModeActions);

    action = new QAction(tr("Pick Element"), this);
    action->setObjectName("aPickElement");
    action->setIconText(tr("Pick"));
    action->setIcon(UIResources::themedIcon(QLatin1String("pick-element.png")));
    action->setCheckable(true);
    action->setToolTip(tr("<b>Pick Element</b><br>"
                          "Select an element for inspection by clicking on it."));
    action->setData(ElementPicking);
    action->setActionGroup(m_interactionModeActions);

    action = new QAction(UIResources::themedIcon(QLatin1String("redirect-input.png")),
                         tr("Redirect Input"), this);
    action->setObjectName("aRedirectInput");
    action->setCheckable(true);
    action->setToolTip(tr("<b>Redirect Input</b><br>"
                          "In this mode all mouse input is redirected directly to the original application,"
                          "so you can control the application directly from within GammaRay."));
    action->setData(InputRedirection);
    action->setActionGroup(m_interactionModeActions);

    action = new QAction(UIResources::themedIcon(QLatin1String("pick-color.png")),
                         tr("Inspect Colors"), this);
    action->setObjectName("aInspectColors");
    action->setCheckable(true);
    action->setToolTip(tr("<b>Inspect Colors</b><br>"
                          "Inspect the RGBA channels of the currently hovered pixel"));
    action->setData(ColorPicking);
    action->setActionGroup(m_interactionModeActions);

    m_zoomOutAction = new QAction(UIResources::themedIcon(QLatin1String("zoom-out.png")), tr(
                                      "Zoom Out"), this);
    m_zoomOutAction->setObjectName("aZoomOut");
    m_zoomOutAction->setShortcutContext(Qt::WidgetShortcut);
    m_zoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    connect(m_zoomOutAction, &QAction::triggered, this, &RemoteViewWidget::zoomOut);
    addAction(m_zoomOutAction); // needed to make the WidgetShortcut context work

    m_zoomInAction = new QAction(UIResources::themedIcon(QLatin1String("zoom-in.png")), tr(
                                     "Zoom In"), this);
    m_zoomInAction->setObjectName("aZoomIn");
    m_zoomInAction->setShortcutContext(Qt::WidgetShortcut);
    m_zoomInAction->setShortcuts(QKeySequence::ZoomIn);
    connect(m_zoomInAction, &QAction::triggered, this, &RemoteViewWidget::zoomIn);
    addAction(m_zoomInAction);

    m_toggleFPSAction = new QAction(tr("Display FPS"), this);
    m_toggleFPSAction->setObjectName("aToggleFPS");
    m_toggleFPSAction->setCheckable(true);
    m_toggleFPSAction->setToolTip("<b>Display FPS</b><br>"
                                  "Shows rate of received frames from debuggee.");
    connect(m_toggleFPSAction, &QAction::toggled, this, &RemoteViewWidget::enableFPS);
    addAction(m_toggleFPSAction);

    updateActions();
}

void RemoteViewWidget::updateActions()
{
    foreach (auto action, m_interactionModeActions->actions()) {
        action->setEnabled(m_frame.isValid());
    }

    Q_ASSERT(!m_zoomLevels.isEmpty());
    const auto zoomLevel = zoomLevelIndex();
    m_zoomOutAction->setEnabled(zoomLevel != 0);
    m_zoomInAction->setEnabled(zoomLevel != m_zoomLevels.size() - 1);
}

void RemoteViewWidget::enableFPS(const bool showFPS)
{
    m_showFps = showFPS;
}

void RemoteViewWidget::updateUserViewport()
{
    if (!isVisible())
        return;

    const auto userViewport = QRectF(QPointF(std::floor(-m_x / m_zoom), std::floor(-m_y / m_zoom)),
                              QSizeF(std::ceil(width() / m_zoom) + 1, std::ceil(height() / m_zoom) + 1));

    // When We update the viewport by zooming out we need one additional
    // frameupdate from the application before stopping to send userViewports,
    // or otherwise the newly appeared area stays empty :(

    // If we would skip, but need an extra update
    if (userViewport.contains(frame().viewRect()) && m_extraViewportUpdateNeeded) {
        m_extraViewportUpdateNeeded = false;
        m_interface->sendUserViewport(userViewport);
        return;
    }

    // Regular skip
    if (userViewport.contains(frame().viewRect())) {
        return;
    }

    // Refill extra-update again once we needed a real update
    m_extraViewportUpdateNeeded = true;
    m_interface->sendUserViewport(userViewport);
}

const RemoteViewFrame &RemoteViewWidget::frame() const
{
    return m_frame;
}

RemoteViewInterface *RemoteViewWidget::remoteViewInterface() const
{
    return m_interface.data();
}

void RemoteViewWidget::pickElementId(const QModelIndex &index)
{
    const GammaRay::ObjectId id = index.data(ObjectModel::ObjectIdRole).value<GammaRay::ObjectId>();
    m_interface->pickElementId(id);
}

void RemoteViewWidget::elementsAtReceived(const GammaRay::ObjectIds &ids, int bestCandidate)
{
    Q_ASSERT(!ids.isEmpty());

    m_pickProxyModel->setIds(ids);

    if (ids.count() == 1) {
        m_interface->pickElementId(ids.first());
    } else {
        const int candidate = bestCandidate == -1 ? 0 : bestCandidate;
        auto *dlg = new ModelPickerDialog(window());
        m_invisibleItemsProxyModel->setSourceModel(m_pickProxyModel);
        m_invisibleItemsProxyModel->setFlagRole(flagRole());
        m_invisibleItemsProxyModel->setInvisibleMask(invisibleMask());

        dlg->setModel(m_invisibleItemsProxyModel);
        dlg->setCurrentIndex(ObjectModel::ObjectIdRole, QVariant::fromValue(ids[candidate]));
        connect(dlg, &ModelPickerDialog::activated, this, &RemoteViewWidget::pickElementId);
        connect(dlg, &ModelPickerDialog::checkBoxStateChanged, m_invisibleItemsProxyModel, &VisibilityFilterProxyModel::setHideItems);
        dlg->open();
    }
}

void RemoteViewWidget::frameUpdated(const RemoteViewFrame &frame)
{
    if (!m_frame.isValid()) {
        m_frame = frame;
        if (m_initialZoomDone)
            centerView();
        else
            fitToView();
    } else {
        m_frame = frame;
        update();
        m_fps = 1000.0 / m_fpsTimer.elapsed();
        m_fpsTimer.restart();
    }

    updateActions();
    if (m_interactionMode == ColorPicking)
        pickColor();
    emit frameChanged();
    QMetaObject::invokeMethod(m_interface, "clientViewUpdated", Qt::QueuedConnection);
}

int RemoteViewWidget::invisibleMask() const
{
    return m_invisibleMask;
}

void RemoteViewWidget::setInvisibleMask(int invisibleMask)
{
    m_invisibleMask = invisibleMask;
}

bool RemoteViewWidget::hasValidFrame() const
{
    return m_frame.isValid();
}

bool RemoteViewWidget::hasValidCompleteFrame() const
{
    return m_frame.isValid()
            && (m_frame.image().size() / m_frame.image().devicePixelRatio())
            == m_frame.viewRect().size().toSize();
}

int RemoteViewWidget::flagRole() const
{
    return m_flagRole;
}

void RemoteViewWidget::setFlagRole(int flagRole)
{
    m_flagRole = flagRole;
}

QAbstractItemModel *RemoteViewWidget::pickSourceModel() const
{
    return m_pickProxyModel->sourceModel();
}

void RemoteViewWidget::setPickSourceModel(QAbstractItemModel *sourceModel)
{
    if (sourceModel == m_pickProxyModel->sourceModel())
        return;

    m_pickProxyModel->setSourceModel(sourceModel);
}

void RemoteViewWidget::reset()
{
    m_frame = RemoteViewFrame();
    m_hasMeasurement = false;
    update();
    emit frameChanged();
}

void RemoteViewWidget::setUnavailableText(const QString &msg)
{
    m_unavailableText = msg;
    update();
}

QActionGroup *RemoteViewWidget::interactionModeActions() const
{
    return m_interactionModeActions;
}

QAction *RemoteViewWidget::zoomOutAction() const
{
    return m_zoomOutAction;
}

QAction *RemoteViewWidget::zoomInAction() const
{
    return m_zoomInAction;
}

void RemoteViewWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return;

    QDataStream stream(state);
    restoreState(stream);
}

QByteArray RemoteViewWidget::saveState() const
{
    QByteArray data;

    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        saveState(stream);
    }

    return data;
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
        if (zoom - (*it) < delta)
            --index;
    }

    if (m_zoomLevels.at(index) == oldZoom)
        return;

    m_zoom = m_zoomLevels.at(index);
    emit zoomChanged();
    emit zoomLevelChanged(index);
    emit stateChanged();

    m_x = contentWidth() / 2 - (contentWidth() / 2 - m_x) * m_zoom / oldZoom;
    m_y = contentHeight() / 2 - (contentHeight() / 2 - m_y) * m_zoom / oldZoom;

    updateActions();
    updateUserViewport();
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
    const auto scale =
        std::min<double>(1.0,
                         std::min((double)contentWidth() / (double)m_frame.sceneRect().width(),
                                  (double)contentHeight() / (double)m_frame.sceneRect().height()));
    setZoom(scale);
    centerView();
}

void RemoteViewWidget::centerView()
{
    m_x = 0.5 * (contentWidth() - m_frame.sceneRect().width() * m_zoom);
    m_y = 0.5 * (contentHeight() - m_frame.sceneRect().height() * m_zoom);
    updateUserViewport();
    update();
}

QAbstractItemModel *RemoteViewWidget::zoomLevelModel() const
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

    switch (mode) {
    case Measuring:
    case ElementPicking:
        setCursor(Qt::CrossCursor);
        break;
    case ViewInteraction:
        setCursor(Qt::OpenHandCursor);
        break;
    case NoInteraction:
    case InputRedirection:
        setCursor(QCursor());
        break;
    case ColorPicking:
        setCursor(Qt::CrossCursor);
        break;
    }

    m_interactionMode = mode;
    foreach (auto action, m_interactionModeActions->actions()) {
        if (action->data() == mode)
            action->setChecked(true);
    }

    update();
    emit interactionModeChanged();
    emit stateChanged();
}

RemoteViewWidget::InteractionModes RemoteViewWidget::supportedInteractionModes() const
{
    return m_supportedInteractionModes;
}

void RemoteViewWidget::setSupportedInteractionModes(RemoteViewWidget::InteractionModes modes)
{
    m_supportedInteractionModes = modes;
    foreach (auto action, m_interactionModeActions->actions())
        action->setVisible(action->data().toInt() & modes);
}

void RemoteViewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);

    if (!m_frame.isValid()) {
        QWidget::paintEvent(event);
        p.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, m_unavailableText);
        return;
    }

    drawBackground(&p);

    p.save();
    p.setTransform(QTransform::fromTranslate(m_x, m_y));
    if (m_zoom < 1.0) { // We want the preview to look nice when zoomed out,
                        // but need to be able to see single pixels when zoomed in.
        p.setRenderHint(QPainter::SmoothPixmapTransform);
    }

    p.save();
    p.setTransform(QTransform().scale(m_zoom, m_zoom), true);
    p.setTransform(m_frame.transform(), true);
    p.drawImage(QPoint(), m_frame.image());
    p.restore();

    drawDecoration(&p);
    p.restore();

    drawRuler(&p);
    if (m_showFps)
        drawFPS(&p);

    if (m_interactionMode == Measuring && m_hasMeasurement)
        drawMeasureOverlay(&p);
}

void RemoteViewWidget::drawBackground(QPainter *p)
{
    // not really efficient...
    p->fillRect(rect(), m_inactiveBackgroundBrush);
    p->fillRect(m_x, m_y,
                m_zoom * m_frame.viewRect().width(),
                m_zoom * m_frame.viewRect().height(),
                m_activeBackgroundBrush);
}

void RemoteViewWidget::drawDecoration(QPainter *p)
{
    Q_UNUSED(p);
}

static int tickLength(int sourcePos, int labelDistance)
{
    int l = 8;
    if (sourcePos % labelDistance == 0) {
        return 2 * l;
    }
    if (sourcePos % 10 == 0) {
        return l + 2;
    } else if (sourcePos % 5 == 0) {
        return l + 1;
    }
    return l;
}

void RemoteViewWidget::drawRuler(QPainter *p)
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
        if (sourcePos == m_currentMousePosition.x()) {
            p->setPen(selectedPen);
        } else if (sourcePos < 0 || sourcePos > m_frame.viewRect().width()) {
            p->setPen(inactivePen);
        } else {
            p->setPen(activePen);
        }

        const int tickSize = tickLength(sourcePos, sourceLabelDist);
        p->drawLine(i, 0, i, tickSize);

        if (sourcePos % sourceLabelDist == 0) {
            if (sourcePos < 0 || sourcePos > m_frame.viewRect().width()) {
                p->setPen(inactivePen);
            } else {
                p->setPen(activePen);
            }
            p->drawText(i - viewLabelDist / 2, tickSize, viewLabelDist, hRulerHeight - tickSize,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number(sourcePos));
        }
    }
    p->restore();

    // vertical ruler on the right
    p->save();
    p->translate(width() - vRulerWidth, 0);
    for (int i = (m_y % viewTickStep); i < contentHeight(); i += viewTickStep) {
        const int sourcePos = (i - m_y) / m_zoom;
        if (sourcePos == m_currentMousePosition.y()) {
            p->setPen(selectedPen);
        } else if (sourcePos < 0 || sourcePos > m_frame.viewRect().height()) {
            p->setPen(inactivePen);
        } else {
            p->setPen(activePen);
        }

        const int tickSize = tickLength(sourcePos, sourceLabelDist);
        p->drawLine(0, i, tickSize, i);

        if (sourcePos % sourceLabelDist == 0) {
            if (sourcePos < 0 || sourcePos > m_frame.viewRect().height()) {
                p->setPen(inactivePen);
            } else {
                p->setPen(activePen);
            }
            p->drawText(tickSize, i - viewLabelDist / 2, vRulerWidth - tickSize, viewLabelDist,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number(sourcePos));
        }
    }
    p->restore();

    p->setPen(activePen);
    p->drawText(QRect(width() - vRulerWidth, height() - hRulerHeight, vRulerWidth, hRulerHeight),
                QStringLiteral("%1x\n%2").
                    arg(std::floor(m_currentMousePosition.x())).
                    arg(std::floor(m_currentMousePosition.y())),
                Qt::AlignHCenter | Qt::AlignVCenter);
    p->restore();
}

void RemoteViewWidget::drawFPS(QPainter *p)
{
    p->save();

    const int hRulerHeight = horizontalRulerHeight();
    const int vRulerWidth = verticalRulerWidth();
    QFontMetrics metrics(p->font());
    const int barWidth = 20;

    QString fps = QString::number(m_fps, 'g', 3) + " fps";
    const QRect textrect(width()  - vRulerWidth  - metrics.width(fps) - 5,
                         height() - hRulerHeight - metrics.height()   - 5,
                         metrics.width(fps) + 2,
                         metrics.height()   + 2);
    p->drawText(textrect, Qt::AlignRight, fps);

    p->setBrush(QBrush(QColor(51, 51, 51, 170)));
    p->setPen(Qt::NoPen);
    p->drawRect(QRect(width() - vRulerWidth - barWidth - 5,
                      height() - hRulerHeight - metrics.height() - 5,
                      barWidth,
                      -10.0 * m_fps));

    p->restore();
}

int RemoteViewWidget::viewTickLabelDistance() const
{
    const auto maxLabel = std::max(m_frame.viewRect().width(), m_frame.viewRect().height());
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
    while (sourceDist > m_tickLabelDists.last())
        m_tickLabelDists.push_back(m_tickLabelDists.at(m_tickLabelDists.size() - 4) * 10);

    const auto it = std::lower_bound(m_tickLabelDists.constBegin(),
                                     m_tickLabelDists.constEnd(), sourceDist);
    return *it;
}

void RemoteViewWidget::drawMeasureOverlay(QPainter *p)
{
    p->save();

    p->setCompositionMode(QPainter::CompositionMode_Difference);
    auto pen = QPen(QColor(255, 255, 255, 170));
    p->setPen(pen);

    const auto startPos = mapFromSource(m_measurementStartPosition);
    const auto endPos = mapFromSource(m_measurementEndPosition);
    const QPoint hOffset(5, 0);
    const QPoint vOffset(0, 5);

    p->drawLine(startPos - hOffset, startPos + hOffset);
    p->drawLine(startPos - vOffset, startPos + vOffset);

    p->drawLine(endPos - hOffset, endPos + hOffset);
    p->drawLine(endPos - vOffset, endPos + vOffset);

    p->drawLine(startPos, endPos);

    pen.setStyle(Qt::DotLine);
    p->setPen(pen);
    p->drawLine(startPos, QPoint(endPos.x(), startPos.y()));
    p->drawLine(QPoint(endPos.x(), startPos.y()), endPos);
    p->restore();

    // start and end labels
    const QPoint startLabelDir(startPos.x() < endPos.x() ? -1 : 1,
                               startPos.y() < endPos.y() ? -1 : 1);
    const QPoint endLabelDir(-startLabelDir.x(), -startLabelDir.y());
    drawMeasurementLabel(p, startPos, startLabelDir,
                         QStringLiteral("x: %1 y: %2").
                             arg(m_measurementStartPosition.x()).
                             arg(m_measurementStartPosition.y()));
    if (endPos != startPos) {
        drawMeasurementLabel(p, endPos, endLabelDir,
                             QStringLiteral("x: %1 y: %2").
                                 arg(m_measurementEndPosition.x()).
                                 arg(m_measurementEndPosition.y()));
    }

    // distance label
    const auto dPos = QPoint(startPos + endPos) / 2;
    const auto d = QLineF(m_measurementStartPosition, m_measurementEndPosition).length();
    if (d > 0) {
        const QPoint dDir(startLabelDir.x(), endLabelDir.y());
        drawMeasurementLabel(p, dPos, dDir, QStringLiteral("%1px").arg(d, 0, 'f', 2));
    }

    // x/y length labels, if there is enough space
    const auto xLength = std::abs(m_measurementStartPosition.x() - m_measurementEndPosition.x());
    const auto yLength = std::abs(m_measurementStartPosition.y() - m_measurementEndPosition.y());

    const auto xDiff = std::abs(endPos.x() - startPos.x());
    if (xDiff > fontMetrics().height() * 2 && xLength > 0 && yLength > 0) {
        const auto xPos = QPoint(dPos.x(), startPos.y());
        const QPoint xDir = QPoint(-startLabelDir.x(), startLabelDir.y());
        drawMeasurementLabel(p, xPos, xDir, QStringLiteral("x: %1px").arg(xLength));
    }

    const auto yDiff = std::abs(endPos.y() - startPos.y());
    if (yDiff > fontMetrics().height() * 2 && xLength > 0 && yLength > 0) {
        const auto yPos = QPoint(endPos.x(), dPos.y());
        const QPoint yDir = QPoint(endLabelDir.x(), -endLabelDir.y());
        drawMeasurementLabel(p, yPos, yDir, QStringLiteral("y: %1px").arg(yLength));
    }
}

void RemoteViewWidget::drawMeasurementLabel(QPainter *p, QPoint pos, QPoint dir,
                                            const QString &text)
{
    p->save();
    static const auto margin = 2;
    const auto height = fontMetrics().height() + (2 * margin);
    const auto width = fontMetrics().width(text) + (2 * margin);

    QRect r(pos.x(), pos.y(), width * dir.x(), height * dir.y());
    r = r.normalized();
    r = r.translated(dir * 5);

    p->setPen(palette().color(QPalette::Text));
    p->setBrush(palette().background());
    p->drawRect(r);
    p->drawText(r, Qt::AlignCenter, text);
    p->restore();
}

QPoint RemoteViewWidget::mapToSource(QPoint pos) const
{
    return (pos - QPoint(m_x, m_y)) / m_zoom;
}

QPointF RemoteViewWidget::mapToSource(QPointF pos) const
{
    return (pos - QPointF(m_x, m_y)) / m_zoom;
}

QRectF RemoteViewWidget::mapToSource(const QRectF &r) const
{
    return QRectF(mapToSource(r.topLeft()), mapToSource(r.bottomRight()));
}

QPoint RemoteViewWidget::mapFromSource(QPoint pos) const
{
    return pos * m_zoom + QPoint(m_x, m_y);
}

QPointF RemoteViewWidget::mapFromSource(QPointF pos) const
{
    return pos * m_zoom + QPointF(m_x, m_y);
}

QRectF RemoteViewWidget::mapFromSource(const QRect& rect) const
{
    return {rect.x() * m_zoom, rect.y() * m_zoom, rect.width() * m_zoom, rect.height() * m_zoom};
}

QTouchEvent::TouchPoint RemoteViewWidget::mapToSource(const QTouchEvent::TouchPoint &point)
{
    QTouchEvent::TouchPoint p;

    p.setFlags(point.flags());
    p.setId(point.id());
    p.setPressure(point.pressure());
    p.setState(point.state());

    p.setStartPos(mapToSource(point.startPos()));
    p.setLastPos(mapToSource(point.lastPos()));
    p.setPos(mapToSource(point.pos())); // relative
    p.setRect(mapToSource(point.rect()));

    p.setStartNormalizedPos(mapToSource(point.startNormalizedPos()));
    p.setLastNormalizedPos(mapToSource(point.lastNormalizedPos()));
    p.setNormalizedPos(mapToSource(point.normalizedPos()));

    p.setStartScenePos(mapToSource(point.startScenePos()));
    p.setLastScenePos(mapToSource(point.lastScenePos()));
    p.setScenePos(mapToSource(point.scenePos()));
    p.setSceneRect(mapToSource(point.sceneRect()));

    // These may not have the right x,y but i don't think there is a way to get the right value.
    // On the other hand, it's not a problem, probably
    p.setStartScreenPos(mapToSource(point.startScreenPos()));
    p.setLastScreenPos(mapToSource(point.lastScreenPos()));
    p.setScreenPos(mapToSource(point.screenPos()));
    p.setScreenRect(mapToSource(point.screenRect()));

    return p;
}

void RemoteViewWidget::restoreState(QDataStream &stream)
{
    stream.setVersion(QDataStream::Qt_4_8);

    qint32 version;
    InteractionMode interactionMode = m_interactionMode;
    double zoom = m_zoom;

    stream >> version;

    switch (version) {
    case RemoteViewWidgetStateVersion: {
        stream >> interactionMode;
        stream >> zoom;
        break;
    }
    }

    setInteractionMode(interactionMode);
    setZoom(zoom);
    m_initialZoomDone = true;
}

void RemoteViewWidget::saveState(QDataStream &stream) const
{
    stream.setVersion(QDataStream::Qt_4_8);

    stream << RemoteViewWidgetStateVersion;

    switch (RemoteViewWidgetStateVersion) {
    case 1: {
        stream << m_interactionMode;
        stream << m_zoom;
        break;
    }
    }
}

void RemoteViewWidget::clampPanPosition()
{
    if (m_x > width() / 2)
        m_x = width() / 2;
    else if (m_x + m_frame.sceneRect().width() * m_zoom < width() / 2.0)
        m_x = width() / 2 - m_frame.sceneRect().width() * m_zoom;
    if (m_y > height() / 2)
        m_y = height() / 2;
    else if (m_y + m_frame.sceneRect().height() * m_zoom < height() / 2.0)
        m_y = height() / 2 - m_frame.sceneRect().height() * m_zoom;
}

void RemoteViewWidget::resizeEvent(QResizeEvent *event)
{
    m_x += 0.5 * (event->size().width() - event->oldSize().width());
    m_y += 0.5 * (event->size().height() - event->oldSize().height());

    updateUserViewport();
    QWidget::resizeEvent(event);
}

void RemoteViewWidget::mousePressEvent(QMouseEvent *event)
{
    m_currentMousePosition = mapToSource(QPointF(event->pos()));

    switch (m_interactionMode) {
    case NoInteraction:
        break;
    case ViewInteraction:
        m_mouseDownPosition = event->pos() - QPoint(m_x, m_y);
        if ((m_supportedInteractionModes & ElementPicking)) {
            if ((event->modifiers() & Qt::ShiftModifier) && (event->modifiers() & Qt::ControlModifier)) {
                m_interface->requestElementsAt(mapToSource(event->pos()), RemoteViewInterface::RequestAll);
            }
            else if ((event->modifiers() & Qt::ControlModifier)) {
                m_interface->requestElementsAt(mapToSource(event->pos()), RemoteViewInterface::RequestBest);
            }
        }
        if (event->buttons() & Qt::LeftButton)
            setCursor(Qt::ClosedHandCursor);
        break;
    case Measuring:
        if (event->buttons() & Qt::LeftButton) {
            m_hasMeasurement = true;
            m_measurementStartPosition = mapToSource(event->pos());
            m_measurementEndPosition = mapToSource(event->pos());
            update();
        }
        break;
    case ElementPicking:
        if (event->buttons() & Qt::LeftButton) {
            if ((event->modifiers() & Qt::ShiftModifier) && (event->modifiers() & Qt::ControlModifier)) {
                m_interface->requestElementsAt(mapToSource(event->pos()), RemoteViewInterface::RequestAll);
            }
            else {
                m_interface->requestElementsAt(mapToSource(event->pos()), RemoteViewInterface::RequestBest);
            }
        }
        break;
    case InputRedirection:
        sendMouseEvent(event);
        break;
    case ColorPicking:
        break;
    }

    QWidget::mousePressEvent(event);
}

void RemoteViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_currentMousePosition = mapToSource(QPointF(event->pos()));

    switch (m_interactionMode) {
    case NoInteraction:
    case ElementPicking:
        break;
    case ViewInteraction:
        setCursor(Qt::OpenHandCursor);
        break;
    case Measuring:
        if (event->buttons() & Qt::LeftButton)
            m_measurementEndPosition = mapToSource(event->pos());
        break;
    case InputRedirection:
        sendMouseEvent(event);
        break;
    case ColorPicking:
        break;
    }

    QWidget::mouseReleaseEvent(event);
}

void RemoteViewWidget::updatePickerVisibility() const
{
    QPointF sourceCoordinates = frame().transform().inverted().map(QPointF(m_currentMousePosition)); // for quick view, transform is needed
    QPoint sourceCoordinatesInt = QPoint(std::floor(sourceCoordinates.x()), std::floor(sourceCoordinates.y()));
    if (frame().image().rect().contains(sourceCoordinatesInt)) {
        m_trailingColorLabel->show();
    } else {
        m_trailingColorLabel->hide();
    }
}

void RemoteViewWidget::pickColor() const
{
    QPointF sourceCoordinates = frame().transform().inverted().map(QPointF(m_currentMousePosition)); // for quick view, transform is needed
    QPoint sourceCoordinatesInt = QPoint(std::floor(sourceCoordinates.x()), std::floor(sourceCoordinates.y()));
    if (frame().image().rect().contains(sourceCoordinatesInt)) {
        m_trailingColorLabel->setPickedColor(frame().image().pixel(sourceCoordinatesInt));
    } else {
        m_trailingColorLabel->setPickedColor(Qt::transparent);
    }
}

void RemoteViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_currentMousePosition = mapToSource(QPointF(event->pos()));

    switch (m_interactionMode) {
    case NoInteraction:
    case ElementPicking:
        break;
    case ViewInteraction:
        if (event->buttons() != Qt::LeftButton) {
            break;
        }
        m_x = event->x() - m_mouseDownPosition.x();
        m_y = event->y() - m_mouseDownPosition.y();
        clampPanPosition();
        updateUserViewport();
        break;
    case Measuring:
        if (event->buttons() & Qt::LeftButton) {
            m_measurementEndPosition = mapToSource(event->pos());
        }
        break;
    case InputRedirection:
        sendMouseEvent(event);
        break;
    case ColorPicking:
        // label should be always fully inside the remoteviewwidget
        auto labelPosition = event->pos() + QPoint(4, 4);
        // flip to top if it would stick out bottom end of remoteviewwidget
        if ((labelPosition.y() + m_trailingColorLabel->height()) > this->height())
            labelPosition = labelPosition - QPoint(0, 8) - QPoint(0, m_trailingColorLabel->height());
        // flip to left if it would stick out right side of remoteviewwidget
        if ((labelPosition.x() + m_trailingColorLabel->width()) > this->width())
            labelPosition = labelPosition - QPoint(8, 0) - QPoint(m_trailingColorLabel->width(), 0);
        m_trailingColorLabel->move(labelPosition);
        updatePickerVisibility();
        pickColor();
        break;
    }
    update();
}

void RemoteViewWidget::wheelEvent(QWheelEvent *event)
{
    switch (m_interactionMode) {
    case NoInteraction:
        break;
    case ViewInteraction:
    case ElementPicking:
    case Measuring:
    case ColorPicking:
        if (event->modifiers() & Qt::ControlModifier && event->orientation() == Qt::Vertical) {
            if (event->delta() > 0) {
                zoomIn();
            } else {
                zoomOut();
            }
        } else {
            if (event->orientation() == Qt::Vertical) {
                m_y += event->delta();
            } else {
                m_x += event->delta();
            }
            clampPanPosition();
            updateUserViewport();
        }
        m_currentMousePosition = mapToSource(QPointF(event->pos()));
        if (m_interactionMode == ColorPicking) {
            updatePickerVisibility();
            pickColor();
        }
        update();
        break;
    case InputRedirection:
        sendWheelEvent(event);
        break;
    }

    QWidget::wheelEvent(event);
}

void RemoteViewWidget::keyPressEvent(QKeyEvent *event)
{
    switch (m_interactionMode) {
    case NoInteraction:
    case ViewInteraction:
    case ElementPicking:
    case Measuring:
        break;
    case InputRedirection:
        sendKeyEvent(event);
        break;
    case ColorPicking:
        if (event->matches(QKeySequence::Copy)){
            QMimeData *data = new QMimeData();
            data->setColorData(m_trailingColorLabel->pickedColor());
            qApp->clipboard()->setMimeData(data);
            qApp->clipboard()->setText(m_trailingColorLabel->pickedColor().name());
        }
    }
    QWidget::keyPressEvent(event);
}

void RemoteViewWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (m_interactionMode) {
    case InputRedirection:
        sendKeyEvent(event);
        break;
    default:
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void RemoteViewWidget::showEvent(QShowEvent *event)
{
    if (m_interface) {
        m_interface->setViewActive(true);
        updateUserViewport();
    }
    QWidget::showEvent(event);
}

void RemoteViewWidget::hideEvent(QHideEvent *event)
{
    if (Endpoint::isConnected()) {
        if (m_interface)
            m_interface->setViewActive(false);
    }
    QWidget::hideEvent(event);
}

void RemoteViewWidget::contextMenuEvent(QContextMenuEvent *event)
{
    switch (m_interactionMode) {
    case ViewInteraction:
    case ElementPicking:
    case Measuring:
    case ColorPicking:
    {
        QMenu menu;
        menu.addActions(m_interactionModeActions->actions());
        menu.addSeparator();
        menu.addAction(m_zoomOutAction);
        menu.addAction(m_zoomInAction);
        if (!qgetenv("GAMMARAY_DEVELOPERMODE").isEmpty()) {
            menu.addSeparator();
            menu.addAction(m_toggleFPSAction);
        }
        menu.exec(event->globalPos());
        break;
    }
    case NoInteraction:
    case InputRedirection:
        QWidget::contextMenuEvent(event);
        break;
    }
}

void RemoteViewWidget::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    switch (m_interactionMode) {
    case ViewInteraction:
    case ElementPicking:
    case Measuring:
    case NoInteraction:
    case InputRedirection:
        break;
    case ColorPicking:
        m_trailingColorLabel->show();
        break;
    }
}

void RemoteViewWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    switch (m_interactionMode) {
    case ViewInteraction:
    case ElementPicking:
    case Measuring:
    case NoInteraction:
    case InputRedirection:
    case ColorPicking:
        m_trailingColorLabel->hide();
        break;
    }
}

bool RemoteViewWidget::eventFilter(QObject *receiver, QEvent *event)
{
    if (receiver == window()) {
        if (m_interface) {
            if (event->type() == QEvent::Show) {
                m_interface->setViewActive(isVisible());
            } else if (event->type() == QEvent::Hide) {
                m_interface->setViewActive(false);
            }
        }
    }

    return QWidget::eventFilter(receiver, event);
}

bool RemoteViewWidget::event(QEvent *event)
{
    if (m_interactionMode == InputRedirection) {
        switch (event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchCancel:
        case QEvent::TouchEnd:
        case QEvent::TouchUpdate:
            sendTouchEvent(static_cast<QTouchEvent *>(event));
            return true;

        default:
            break;
        }
    }

    return QWidget::event(event);
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
    return fontMetrics().width(QString::number(m_frame.sceneRect().height())) + 24; // 2* tick length + some margin
}

int RemoteViewWidget::horizontalRulerHeight() const
{
    return fontMetrics().height() + 20; // 2 * tick length + some margin
}

void RemoteViewWidget::interactionActionTriggered(QAction *action)
{
    Q_ASSERT(action);
    setInteractionMode(static_cast<InteractionMode>(action->data().toInt()));
}

void RemoteViewWidget::sendMouseEvent(QMouseEvent *event)
{
    m_interface->sendMouseEvent(event->type(), mapToSource(event->pos()),
                                event->button(), event->buttons(), event->modifiers());
}

void RemoteViewWidget::sendKeyEvent(QKeyEvent *event)
{
    m_interface->sendKeyEvent(event->type(), event->key(), event->modifiers(),
                              event->text(), event->isAutoRepeat(), event->count());
}

void RemoteViewWidget::sendWheelEvent(QWheelEvent *event)
{
    auto angleDelta = event->angleDelta();
    auto pixelDelta = event->pixelDelta();
    m_interface->sendWheelEvent(mapToSource(event->pos()), pixelDelta, angleDelta,
                                event->buttons(), event->modifiers());
}

void RemoteViewWidget::sendTouchEvent(QTouchEvent *event)
{
    event->accept();

    QList<QTouchEvent::TouchPoint> touchPoints;
    foreach (const QTouchEvent::TouchPoint &point, event->touchPoints()) {
        touchPoints << mapToSource(point);
    }

    QTouchDevice::Capabilities caps = event->device()->capabilities();
    caps &= ~QTouchDevice::RawPositions; //we don't have a way to meaningfully map the raw positions to the source
    caps &= ~QTouchDevice::Velocity; //neither for velocity

    m_interface->sendTouchEvent(event->type(),
                                event->device()->type(),
                                caps,
                                event->device()->maximumTouchPoints(),
                                event->modifiers(), event->touchPointStates(), touchPoints);
}
