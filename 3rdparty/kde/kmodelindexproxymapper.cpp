/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "kmodelindexproxymapper.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QPointer>
#include <QtCore/QDebug>
#include <QAbstractProxyModel>
#include <QItemSelectionModel>

// super dirty backward compat hack with Qt4
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtCore/QWeakPointer>
#define QPointer QWeakPointer
#endif

class KModelIndexProxyMapperPrivate
{
    KModelIndexProxyMapperPrivate(const QAbstractItemModel *leftModel, const QAbstractItemModel *rightModel, KModelIndexProxyMapper *qq)
        : q_ptr(qq), m_leftModel(leftModel), m_rightModel(rightModel)
    {
        createProxyChain();
    }

    void createProxyChain();
    bool assertValid();

    bool assertSelectionValid(const QItemSelection &selection) const
    {
        Q_FOREACH (const QItemSelectionRange &range, selection) {
            if (!range.isValid()) {
                qDebug() << selection << m_leftModel << m_rightModel << m_proxyChainDown << m_proxyChainUp;
            }
            Q_ASSERT(range.isValid());
        }
        return true;
    }

    Q_DECLARE_PUBLIC(KModelIndexProxyMapper)
    KModelIndexProxyMapper *const q_ptr;

    QList<QPointer<const QAbstractProxyModel> > m_proxyChainUp;
    QList<QPointer<const QAbstractProxyModel> > m_proxyChainDown;

    QPointer<const QAbstractItemModel> m_leftModel;
    QPointer<const QAbstractItemModel> m_rightModel;
};

/*

  The idea here is that <tt>this</tt> selection model and proxySelectionModel might be in different parts of the
  proxy chain. We need to build up to two chains of proxy models to create mappings between them.

  Example 1:

     Root model
          |
        /    \
    Proxy 1   Proxy 3
       |       |
    Proxy 2   Proxy 4

  Need Proxy 1 and Proxy 2 in one chain, and Proxy 3 and 4 in the other.

  Example 2:

     Root model
          |
        Proxy 1
          |
        Proxy 2
        /     \
    Proxy 3   Proxy 6
       |       |
    Proxy 4   Proxy 7
       |
    Proxy 5

  We first build the chain from 1 to 5, then start building the chain from 7 to 1. We stop when we find that proxy 2 is
  already in the first chain.

  Stephen Kelly, 30 March 2010.
*/

void KModelIndexProxyMapperPrivate::createProxyChain()
{
    QPointer<const QAbstractItemModel> targetModel = m_rightModel;

    if (!targetModel) {
        return;
    }

    if (m_leftModel == targetModel) {
        return;
    }

    QList<QPointer<const QAbstractProxyModel> > proxyChainDown;
    QPointer<const QAbstractProxyModel> selectionTargetProxyModel = qobject_cast<const QAbstractProxyModel *>(targetModel.data());
    while (selectionTargetProxyModel) {
        proxyChainDown.prepend(selectionTargetProxyModel);

        selectionTargetProxyModel = qobject_cast<const QAbstractProxyModel *>(selectionTargetProxyModel.data()->sourceModel());

        if (selectionTargetProxyModel.data() == m_leftModel.data()) {
            m_proxyChainDown = proxyChainDown;
            return;
        }
    }

    QPointer<const QAbstractItemModel> sourceModel = m_leftModel;
    QPointer<const QAbstractProxyModel> sourceProxyModel = qobject_cast<const QAbstractProxyModel *>(sourceModel.data());

    while (sourceProxyModel) {
        m_proxyChainUp.append(sourceProxyModel);

        sourceProxyModel = qobject_cast<const QAbstractProxyModel *>(sourceProxyModel.data()->sourceModel());

        const int targetIndex = proxyChainDown.indexOf(sourceProxyModel);

        if (targetIndex != -1) {
            m_proxyChainDown = proxyChainDown.mid(targetIndex + 1, proxyChainDown.size());
            return;
        }
    }
    m_proxyChainDown = proxyChainDown;
    Q_ASSERT(assertValid());
}

bool KModelIndexProxyMapperPrivate::assertValid()
{
    if (m_proxyChainDown.isEmpty()) {
        Q_ASSERT(!m_proxyChainUp.isEmpty());
        Q_ASSERT(m_proxyChainUp.last().data()->sourceModel() == m_rightModel.data());
    } else if (m_proxyChainUp.isEmpty()) {
        Q_ASSERT(!m_proxyChainDown.isEmpty());
        Q_ASSERT(m_proxyChainDown.first().data()->sourceModel() == m_leftModel.data());
    } else {
        Q_ASSERT(m_proxyChainDown.first().data()->sourceModel() == m_proxyChainUp.last().data()->sourceModel());
    }
    return true;
}

KModelIndexProxyMapper::KModelIndexProxyMapper(const QAbstractItemModel *leftModel, const QAbstractItemModel *rightModel, QObject *parent)
    : QObject(parent), d_ptr(new KModelIndexProxyMapperPrivate(leftModel, rightModel, this))
{

}

KModelIndexProxyMapper::~KModelIndexProxyMapper()
{
    delete d_ptr;
}

QModelIndex KModelIndexProxyMapper::mapLeftToRight(const QModelIndex &index) const
{
    const QItemSelection selection = mapSelectionLeftToRight(QItemSelection(index, index));
    if (selection.isEmpty()) {
        return QModelIndex();
    }

    return selection.indexes().first();
}

QModelIndex KModelIndexProxyMapper::mapRightToLeft(const QModelIndex &index) const
{
    const QItemSelection selection = mapSelectionRightToLeft(QItemSelection(index, index));
    if (selection.isEmpty()) {
        return QModelIndex();
    }

    return selection.indexes().first();
}

// QAbstractProxyModel::mapSelectionFromSource creates invalid ranges to we filter
// those out manually in a loop. Hopefully fixed in Qt 4.7.2, so we ifdef it out.
// http://qt.gitorious.org/qt/qt/merge_requests/2474
// http://qt.gitorious.org/qt/qt/merge_requests/831
#if QT_VERSION < 0x040702
#define RANGE_FIX_HACK
#endif

#ifdef RANGE_FIX_HACK
static QItemSelection removeInvalidRanges(const QItemSelection &selection)
{
    QItemSelection result;
    Q_FOREACH (const QItemSelectionRange &range, selection) {
        if (!range.isValid()) {
            continue;
        }
        result << range;
    }
    return result;
}
#endif

QItemSelection KModelIndexProxyMapper::mapSelectionLeftToRight(const QItemSelection &selection) const
{
    Q_D(const KModelIndexProxyMapper);

    if (selection.isEmpty()) {
        return QItemSelection();
    }

    if (selection.first().model() != d->m_leftModel.data()) {
        qDebug() << "FAIL" << selection.first().model() << d->m_leftModel << d->m_rightModel;
    }
    Q_ASSERT(selection.first().model() == d->m_leftModel.data());

    QItemSelection seekSelection = selection;
    Q_ASSERT(d->assertSelectionValid(seekSelection));
    QListIterator<QPointer<const QAbstractProxyModel> > iUp(d->m_proxyChainUp);

    while (iUp.hasNext()) {
        const QPointer<const QAbstractProxyModel> proxy = iUp.next();
        if (!proxy) {
            return QItemSelection();
        }
        seekSelection = proxy.data()->mapSelectionToSource(seekSelection);

#ifdef RANGE_FIX_HACK
        seekSelection = removeInvalidRanges(seekSelection);
#endif
        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    QListIterator<QPointer<const QAbstractProxyModel> > iDown(d->m_proxyChainDown);

    while (iDown.hasNext()) {
        const QPointer<const QAbstractProxyModel> proxy = iDown.next();
        if (!proxy) {
            return QItemSelection();
        }
        seekSelection = proxy.data()->mapSelectionFromSource(seekSelection);

#ifdef RANGE_FIX_HACK
        seekSelection = removeInvalidRanges(seekSelection);
#endif
        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    Q_ASSERT((!seekSelection.isEmpty() && seekSelection.first().model() == d->m_rightModel.data()) || true);
    return seekSelection;
}

QItemSelection KModelIndexProxyMapper::mapSelectionRightToLeft(const QItemSelection &selection) const
{
    Q_D(const KModelIndexProxyMapper);

    if (selection.isEmpty()) {
        return QItemSelection();
    }

    if (selection.first().model() != d->m_rightModel.data()) {
        qDebug() << "FAIL" << selection.first().model() << d->m_leftModel << d->m_rightModel;
    }
    Q_ASSERT(selection.first().model() == d->m_rightModel.data());

    QItemSelection seekSelection = selection;
    Q_ASSERT(d->assertSelectionValid(seekSelection));
    QListIterator<QPointer<const QAbstractProxyModel> > iDown(d->m_proxyChainDown);

    iDown.toBack();
    while (iDown.hasPrevious()) {
        const QPointer<const QAbstractProxyModel> proxy = iDown.previous();
        if (!proxy) {
            return QItemSelection();
        }
        seekSelection = proxy.data()->mapSelectionToSource(seekSelection);

#ifdef RANGE_FIX_HACK
        seekSelection = removeInvalidRanges(seekSelection);
#endif
        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    QListIterator<QPointer<const QAbstractProxyModel> > iUp(d->m_proxyChainUp);

    iUp.toBack();
    while (iUp.hasPrevious()) {
        const QPointer<const QAbstractProxyModel> proxy = iUp.previous();
        if (!proxy) {
            return QItemSelection();
        }
        seekSelection = proxy.data()->mapSelectionFromSource(seekSelection);

#ifdef RANGE_FIX_HACK
        seekSelection = removeInvalidRanges(seekSelection);
#endif
        Q_ASSERT(d->assertSelectionValid(seekSelection));
    }

    Q_ASSERT((!seekSelection.isEmpty() && seekSelection.first().model() == d->m_leftModel.data()) || true);
    return seekSelection;
}

