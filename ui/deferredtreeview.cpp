/*
 * This file is part of GammaRay, the Qt application inspection and
 * manipulation tool.
 *
 * Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 * Author: Filipe Azevedo <filipe.azevedo@kdab.com>
 *
 * Licensees holding valid commercial KDAB GammaRay licenses may use this file in
 * accordance with GammaRay Commercial License Agreement provided with the Software.
 *
 * Contact info@kdab.com if any conditions of this licensing are not clear to you.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config-gammaray.h>

#include <common/metatypedeclarations.h>

#include "deferredtreeview.h"
#include "deferredtreeview_p.h"

#include <QTimer>

#if defined(HAVE_PRIVATE_QT_HEADERS)
#include <private/qheaderview_p.h>
#endif

using namespace GammaRay;

namespace {
QHeaderView::ResizeMode sectionResizeMode(QHeaderView *header, int logicalIndex)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return header->sectionResizeMode(logicalIndex);
#else
    return header->resizeMode(logicalIndex);
#endif
}

void setSectionResizeMode(QHeaderView *header, int logicalIndex, QHeaderView::ResizeMode mode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    header->setSectionResizeMode(logicalIndex, mode);
#else
    header->setResizeMode(logicalIndex, mode);
#endif
}

#if defined(HAVE_PRIVATE_QT_HEADERS)
QStyle::StateFlag checkStateStyleFlag(Qt::CheckState state) {
    switch (state) {
    case Qt::Unchecked:
        return QStyle::State_Off;
    case Qt::PartiallyChecked:
        return QStyle::State_NoChange;
    case Qt::Checked:
        return QStyle::State_On;
    }

    return QStyle::State_None;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void recalcSectionStartPos(QHeaderViewPrivate *d) {
    int pixelpos = 0;
    for (QVector<QHeaderViewPrivate::SectionItem>::const_iterator i = d->sectionItems.constBegin();
         i != d->sectionItems.constEnd(); ++i) {
        i->calculated_startpos = pixelpos; // write into const mutable
        pixelpos += i->size;
    }
    d->sectionStartposRecalc = false;
}

bool isFirstVisibleSection(QHeaderViewPrivate *d, int section) {
    if (d->sectionStartposRecalc)
        recalcSectionStartPos(d);
    const QHeaderViewPrivate::SectionItem &item = d->sectionItems.at(section);
    return item.size > 0 && item.calculated_startpos == 0;
}

bool isLastVisibleSection(QHeaderViewPrivate *d, int section) {
    if (d->sectionStartposRecalc)
        recalcSectionStartPos(d);
    const QHeaderViewPrivate::SectionItem &item = d->sectionItems.at(section);
    return item.size > 0 && item.calculatedEndPos() == d->length;
}
#endif

bool isSectionSelected(QHeaderViewPrivate *d, int section) {
    int i = d->section * 2;
    if (i < 0 || i >= d->sectionSelected.count())
        return false;
    if (d->sectionSelected.testBit(i)) // if the value was cached
        return d->sectionSelected.testBit(i + 1);
    bool s = false;
    if (d->orientation == Qt::Horizontal)
        s = d->isColumnSelected(section);
    else
        s = d->isRowSelected(section);
    d->sectionSelected.setBit(i + 1, s); // selection state
    d->sectionSelected.setBit(i, true); // cache state
    return s;
}
#endif
}

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
}

bool HeaderView::isState(State state) const
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    QHeaderViewPrivate *d = reinterpret_cast<QHeaderViewPrivate *>(d_ptr.data());
    return d->state == QHeaderViewPrivate::State(state);
#else
    Q_UNUSED(state);
    return false;
#endif
}

bool HeaderView::isSectionCheckable(int logicalIndex) const
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    return model()
            ? model()->headerData(logicalIndex, orientation(), Qt::CheckStateRole).canConvert<Qt::CheckState>()
            : false;
#else
    Q_UNUSED(logicalIndex);
    return false;
#endif
}

Qt::CheckState HeaderView::sectionCheckState(int logicalIndex) const
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    return model()
            ? model()->headerData(logicalIndex, orientation(), Qt::CheckStateRole).value<Qt::CheckState>()
            : Qt::Unchecked;
#else
    Q_UNUSED(logicalIndex);
    return Qt::Unchecked;
#endif
}

void HeaderView::setSectionCheckState(int logicalIndex, Qt::CheckState state)
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    if (model())
        model()->setHeaderData(logicalIndex, orientation(), state, Qt::CheckStateRole);
#else
    Q_UNUSED(logicalIndex);
    Q_UNUSED(state);
#endif
}

void HeaderView::mousePressEvent(QMouseEvent *event)
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    const int logicalIndex = logicalIndexAt(event->pos());

    if (isSectionCheckable(logicalIndex)) {
        const QRect rect = orientation() == Qt::Horizontal
                ? QRect(QPoint(sectionViewportPosition(logicalIndex), 0), QSize(sectionSize(logicalIndex), viewport()->height()))
                : QRect(QPoint(0, sectionViewportPosition(logicalIndex)), QSize(viewport()->width(), sectionSize(logicalIndex)));
        QStyleOptionHeader opt;
        pimpMyStyleOption(&opt, rect, logicalIndex);

        QStyleOptionHeader subopt = opt;
        subopt.rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);

        if (subopt.rect.isValid()) {
            const QPoint pos = viewport()->mapFromGlobal(QCursor::pos());
            const Qt::CheckState state = sectionCheckState(logicalIndex);

            if (subopt.rect.contains(pos)) {
                setSectionCheckState(logicalIndex, state == Qt::Unchecked || state == Qt::PartiallyChecked ? Qt::Checked : Qt::Unchecked);
                return;
            }
        }
    }
#endif
    QHeaderView::mousePressEvent(event);
}

QSize HeaderView::sectionSizeFromContents(int logicalIndex) const
{
    QSize s = QHeaderView::sectionSizeFromContents(logicalIndex);

#if defined(HAVE_PRIVATE_QT_HEADERS)
    if (isSectionCheckable(logicalIndex)) {
        QStyleOptionButton opt;
        opt.initFrom(this);
        opt.state |= checkStateStyleFlag(sectionCheckState(logicalIndex));
        s = style()->sizeFromContents(QStyle::CT_CheckBox, &opt, s, this);
    }
#endif

    return s;
}

void HeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
#if defined(HAVE_PRIVATE_QT_HEADERS)
    if (!rect.isValid())
        return;

    QStyleOptionHeader opt;
    pimpMyStyleOption(&opt, rect, logicalIndex);

    // draw the section

    {
        // We don't use the genereic rendering method
        // style()->drawControl(QStyle::CE_Header, &opt, painter, this);
        // because we have to indent the header label and header arrow, but not the header background
        // so we can render our check indicator.

        QPointF oldBO = painter->brushOrigin();
        QVariant backgroundBrush = model()->headerData(logicalIndex, orientation(),
                                                        Qt::BackgroundRole);
        if (backgroundBrush.canConvert<QBrush>()) {
            const QBrush brush = qvariant_cast<QBrush>(backgroundBrush);
            if (opt.palette.brush(QPalette::Button) == brush &&
                    opt.palette.brush(QPalette::Window) == brush) {
                painter->setBrushOrigin(opt.rect.topLeft());
            }
        }
        QRegion clipRegion = painter->clipRegion();
        painter->setClipRect(opt.rect);
        style()->proxy()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
        QStyleOptionHeader subopt = opt;
        int checkBoxWidth = 0;
        subopt.rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, this);
        if (isSectionCheckable(logicalIndex) && subopt.rect.isValid()) {
            checkBoxWidth = subopt.rect.width();
            subopt.state |= checkStateStyleFlag(sectionCheckState(logicalIndex));
            style()->proxy()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &subopt, painter, this);
        }
        subopt.rect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
        if (subopt.rect.isValid()) {
            subopt.rect.adjust(checkBoxWidth, 0, 0, 0);
            style()->proxy()->drawControl(QStyle::CE_HeaderLabel, &subopt, painter, this);
        }
        if (opt.sortIndicator != QStyleOptionHeader::None) {
            subopt.rect = style()->subElementRect(QStyle::SE_HeaderArrow, &opt, this);
            style()->proxy()->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &subopt, painter, this);
        }
        painter->setClipRegion(clipRegion);
        painter->setBrushOrigin(oldBO);
    }

#else
    QHeaderView::paintSection(painter, rect, logicalIndex);
#endif
}

void HeaderView::pimpMyStyleOption(QStyleOptionHeader *option, const QRect &rect, int logicalIndex) const
{
    initStyleOption(option);

#if defined(HAVE_PRIVATE_QT_HEADERS)
    QHeaderViewPrivate *d = reinterpret_cast<QHeaderViewPrivate *>(d_ptr.data());

    // get the state of the section
    QStyle::State state = QStyle::State_None;
    if (isEnabled())
        state |= QStyle::State_Enabled;
    if (window()->isActiveWindow())
        state |= QStyle::State_Active;
    if (d->clickableSections) {
        if (logicalIndex == d->hover)
            state |= QStyle::State_MouseOver;
        if (logicalIndex == d->pressed)
            state |= QStyle::State_Sunken;
        else if (d->highlightSelected) {
            if (d->sectionIntersectsSelection(logicalIndex))
                state |= QStyle::State_On;
            if (isSectionSelected(d, logicalIndex))
                state |= QStyle::State_Sunken;
        }

    }
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
        option->sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                            ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

    // setup the style options structure
    QVariant textAlignment = d->model->headerData(logicalIndex, d->orientation,
                                                  Qt::TextAlignmentRole);
    option->rect = rect;
    option->section = logicalIndex;
    option->state |= state;
    option->textAlignment = Qt::Alignment(textAlignment.isValid()
                                      ? Qt::Alignment(textAlignment.toInt())
                                      : d->defaultAlignment);

    option->iconAlignment = Qt::AlignVCenter;
    option->text = d->model->headerData(logicalIndex, d->orientation,
                                    Qt::DisplayRole).toString();

    int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, nullptr, this);

    const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, nullptr, this));
    const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
        margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, this);

    if (d->textElideMode != Qt::ElideNone)
        option->text = option->fontMetrics.elidedText(option->text, d->textElideMode , rect.width() - margin);

    QVariant variant = d->model->headerData(logicalIndex, d->orientation,
                                    Qt::DecorationRole);
    option->icon = qvariant_cast<QIcon>(variant);
    if (option->icon.isNull())
        option->icon = qvariant_cast<QPixmap>(variant);
    QVariant foregroundBrush = d->model->headerData(logicalIndex, d->orientation,
                                                    Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
        option->palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

    // the section position
    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    bool first = isFirstVisibleSection(d, visual);
    bool last = isLastVisibleSection(d, visual);
    if (first && last)
        option->position = QStyleOptionHeader::OnlyOneSection;
    else if (first)
        option->position = QStyleOptionHeader::Beginning;
    else if (last)
        option->position = QStyleOptionHeader::End;
    else
        option->position = QStyleOptionHeader::Middle;
#else
    if (count() == 1)
        option->position = QStyleOptionHeader::OnlyOneSection;
    else if (visual == 0)
        option->position = QStyleOptionHeader::Beginning;
    else if (visual == count() - 1)
        option->position = QStyleOptionHeader::End;
    else
        option->position = QStyleOptionHeader::Middle;
#endif
    option->orientation = d->orientation;
    // the selected position
    bool previousSelected = isSectionSelected(d, this->logicalIndex(visual - 1));
    bool nextSelected =  isSectionSelected(d, this->logicalIndex(visual + 1));
    if (previousSelected && nextSelected)
        option->selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
    else if (previousSelected)
        option->selectedPosition = QStyleOptionHeader::PreviousIsSelected;
    else if (nextSelected)
        option->selectedPosition = QStyleOptionHeader::NextIsSelected;
    else
        option->selectedPosition = QStyleOptionHeader::NotAdjacent;
#else
    Q_UNUSED(rect);
    Q_UNUSED(logicalIndex);
#endif
}

DeferredTreeView::DeferredHeaderProperties::DeferredHeaderProperties()
    : initialized(false)
    , resizeMode(-1)
    , hidden(-1)
{
}

DeferredTreeView::DeferredTreeView(QWidget *parent)
    : QTreeView(parent)
    , m_expandNewContent(false)
    , m_allExpanded(false)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(125);

    setHeader(new HeaderView(header()->orientation(), this));

    // Default QTreeView header properties
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    header()->setSectionsMovable(true);
#else
    header()->setMovable(true);
#endif
    header()->setStretchLastSection(true);
    header()->setDefaultAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    // Custom
    header()->setSortIndicatorShown(true);

    setIndentation(10);
    setSortingEnabled(true);

    connect(header(), SIGNAL(sectionCountChanged(int,int)), SLOT(sectionCountChanged()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void DeferredTreeView::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);

    if (model)
        triggerExpansion(QModelIndex());
}

QHeaderView::ResizeMode DeferredTreeView::deferredResizeMode(int logicalIndex) const
{
    const auto it = m_sectionsProperties.constFind(logicalIndex);
    const int resizeMode = it != m_sectionsProperties.constEnd() ? (*it).resizeMode : -1;
    return resizeMode != -1 ? QHeaderView::ResizeMode(resizeMode) : sectionResizeMode(
        header(), logicalIndex);
}

void DeferredTreeView::setDeferredResizeMode(int logicalIndex, QHeaderView::ResizeMode mode)
{
    auto it = m_sectionsProperties.find(logicalIndex);

    if (it != m_sectionsProperties.end()) {
        (*it).resizeMode = mode;
    } else {
        DeferredHeaderProperties properties;
        properties.resizeMode = mode;
        m_sectionsProperties[logicalIndex] = properties;
    }

    if (logicalIndex < header()->count()) {
        setSectionResizeMode(header(), logicalIndex, mode);
        m_sectionsProperties[logicalIndex].initialized = true;
    }
}

bool DeferredTreeView::deferredHidden(int logicalIndex) const
{
    const auto it = m_sectionsProperties.constFind(logicalIndex);
    const int hidden = it != m_sectionsProperties.constEnd() ? (*it).hidden : -1;
    return hidden != -1 ? hidden == 1 : header()->isSectionHidden(logicalIndex);
}

void DeferredTreeView::setDeferredHidden(int logicalIndex, bool hidden)
{
    auto it = m_sectionsProperties.find(logicalIndex);

    if (it != m_sectionsProperties.end()) {
        (*it).hidden = hidden;
    } else {
        DeferredHeaderProperties properties;
        properties.hidden = hidden ? 1 : 0;
        m_sectionsProperties[logicalIndex] = properties;
    }

    if (logicalIndex < header()->count()) {
        header()->setSectionHidden(logicalIndex, hidden);
        m_sectionsProperties[logicalIndex].initialized = true;
    }
}

bool DeferredTreeView::expandNewContent() const
{
    return m_expandNewContent;
}

void DeferredTreeView::setExpandNewContent(bool expand)
{
    m_expandNewContent = expand;
}

bool DeferredTreeView::stretchLastSection() const
{
    return header()->stretchLastSection();
}

void DeferredTreeView::setStretchLastSection(bool stretch)
{
    header()->setStretchLastSection(stretch);
}

void DeferredTreeView::resetDeferredInitialized()
{
    for (auto it = m_sectionsProperties.begin(), end = m_sectionsProperties.end(); it != end; ++it)
        (*it).initialized = false;
}

void DeferredTreeView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);
    triggerExpansion(parent);
}

void DeferredTreeView::sectionCountChanged()
{
    const int sections = header()->count();

    if (sections == 0) {
        resetDeferredInitialized();
        return;
    }

    for (auto it = m_sectionsProperties.begin(), end = m_sectionsProperties.end(); it != end;
         ++it) {
        if ((*it).initialized)
            continue;

        if (it.key() < sections) {
            if ((*it).resizeMode != -1)
                setSectionResizeMode(header(), it.key(), QHeaderView::ResizeMode((*it).resizeMode));

            if ((*it).hidden)
                header()->setSectionHidden(it.key(), (*it).hidden == 1);

            (*it).initialized = true;
        }
    }
}

void DeferredTreeView::triggerExpansion(const QModelIndex &parent)
{
    if (m_expandNewContent) {
        m_insertedRows << QPersistentModelIndex(parent);
        m_timer->start();
    }
}

void DeferredTreeView::timeout()
{
    const QModelIndex selectedRow = selectionModel()->selectedRows().value(0);

    if (m_allExpanded) {
        for (auto it = m_insertedRows.constBegin(), end = m_insertedRows.constEnd(); it != end;
             ++it) {
            if (it->isValid())
                expand(*it);
        }
    } else {
        m_allExpanded = true;
        expandAll();
    }

    m_insertedRows.clear();

    if (selectedRow.isValid())
        scrollTo(selectedRow);

    emit newContentExpanded();
}
