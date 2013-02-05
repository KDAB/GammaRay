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


#include "klinkitemselectionmodel.h"

#include "kmodelindexproxymapper.h"

#include "kdebug.h"

#include <QItemSelection>

class KLinkItemSelectionModelPrivate
{
public:
    KLinkItemSelectionModelPrivate(KLinkItemSelectionModel *proxySelectionModel, QAbstractItemModel *model,
                                    QItemSelectionModel *linkedItemSelectionModel)
      : q_ptr(proxySelectionModel),
        m_model(model),
        m_linkedItemSelectionModel(linkedItemSelectionModel),
        m_ignoreCurrentChanged(false),
        m_indexMapper(new KModelIndexProxyMapper(model, linkedItemSelectionModel->model(), proxySelectionModel))
    {
    }

    Q_DECLARE_PUBLIC(KLinkItemSelectionModel)
    KLinkItemSelectionModel * const q_ptr;


    bool assertSelectionValid(const QItemSelection &selection) const {
      foreach(const QItemSelectionRange &range, selection) {
        if (!range.isValid()) {
          kDebug() << selection;
        }
        Q_ASSERT(range.isValid());
      }
      return true;
    }

    void sourceSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    QAbstractItemModel * const m_model;
    QItemSelectionModel * const m_linkedItemSelectionModel;
    bool m_ignoreCurrentChanged;
    KModelIndexProxyMapper * const m_indexMapper;
};

KLinkItemSelectionModel::KLinkItemSelectionModel(QAbstractItemModel *model, QItemSelectionModel *proxySelector, QObject *parent)
        : QItemSelectionModel(model, parent),
        d_ptr(new KLinkItemSelectionModelPrivate(this, model, proxySelector))
{
    connect(proxySelector, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(sourceSelectionChanged(QItemSelection,QItemSelection)));
}

KLinkItemSelectionModel::~KLinkItemSelectionModel()
{
    delete d_ptr;
}

void KLinkItemSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
{
    Q_D(KLinkItemSelectionModel);
    // When an item is removed, the current index is set to the top index in the model.
    // That causes a selectionChanged signal with a selection which we do not want.
    if (d->m_ignoreCurrentChanged) {
        return;
    }
    QItemSelectionModel::select(index, command);
    if (index.isValid())
        d->m_linkedItemSelectionModel->select(d->m_indexMapper->mapSelectionLeftToRight(QItemSelection(index, index)), command);
    else {
        d->m_linkedItemSelectionModel->clearSelection();
    }
}

// QAbstractProxyModel::mapSelectionFromSource creates invalid ranges to we filter
// those out manually in a loop. Hopefully fixed in Qt 4.7.2, so we ifdef it out.
// http://qt.gitorious.org/qt/qt/merge_requests/2474
// http://qt.gitorious.org/qt/qt/merge_requests/831
#if QT_VERSION < 0x040702
#define RANGE_FIX_HACK
#endif

#ifdef RANGE_FIX_HACK
static QItemSelection klink_removeInvalidRanges(const QItemSelection &selection)
{
  QItemSelection result;
  Q_FOREACH(const QItemSelectionRange &range, selection)
  {
    if (!range.isValid())
      continue;
    result << range;
  }
  return result;
}
#endif

void KLinkItemSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command)
{
    Q_D(KLinkItemSelectionModel);
    d->m_ignoreCurrentChanged = true;
#ifdef RANGE_FIX_HACK
    QItemSelection _selection = klink_removeInvalidRanges(selection);
#else
    QItemSelection _selection = selection;
#endif
    QItemSelectionModel::select(_selection, command);
    Q_ASSERT(d->assertSelectionValid(_selection));
    QItemSelection mappedSelection = d->m_indexMapper->mapSelectionLeftToRight(_selection);
    Q_ASSERT(d->assertSelectionValid(mappedSelection));
    d->m_linkedItemSelectionModel->select(mappedSelection, command);
    d->m_ignoreCurrentChanged = false;
}

void KLinkItemSelectionModelPrivate::sourceSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_Q(KLinkItemSelectionModel);
#ifdef RANGE_FIX_HACK
    QItemSelection _selected = klink_removeInvalidRanges(selected);
    QItemSelection _deselected = klink_removeInvalidRanges(deselected);
#else
    QItemSelection _selected = selected;
    QItemSelection _deselected = deselected;
#endif
    Q_ASSERT(assertSelectionValid(_selected));
    Q_ASSERT(assertSelectionValid(_deselected));
    const QItemSelection mappedDeselection = m_indexMapper->mapSelectionRightToLeft(_deselected);
    const QItemSelection mappedSelection = m_indexMapper->mapSelectionRightToLeft(_selected);

    q->QItemSelectionModel::select(mappedDeselection, QItemSelectionModel::Deselect);
    q->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::Select);
}

#include "klinkitemselectionmodel.moc"
