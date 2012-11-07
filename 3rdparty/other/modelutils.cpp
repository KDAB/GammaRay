#include "modelutils.h"

#include <QAbstractItemModel>

using namespace GammaRay;

QModelIndexList ModelUtils::match(const QAbstractItemModel* model, const QModelIndex& start,
    int role, bool (*accept)(const QVariant&), int hits, Qt::MatchFlags flags)
{
  if (!model || !start.isValid() || role < 0)
    return QModelIndexList();

  const QModelIndex parentIndex = model->parent(start);
  bool recurse = flags & Qt::MatchRecursive;
  bool wrap = flags & Qt::MatchWrap;
  bool allHits = (hits == -1);
  int from = start.row();
  int to = model->rowCount(parentIndex);

  QModelIndexList result;

  // iterates twice if wrapping
  for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {
    for (int r = from; (r < to) && (allHits || result.count() < hits); ++r) {
      QModelIndex idx = model->index(r, start.column(), parentIndex);
      if (!idx.isValid())
        continue;

      const QVariant v = model->data(idx, role);
      if (accept(v))
        result << idx;

      // search the hierarchy
      if (recurse && model->hasChildren(idx)) {
        result += match(model, model->index(0, idx.column(), idx), role,
                        accept, (allHits ? -1 : hits - result.count()), flags);
      }
    }

    // prepare for the next iteration
    from = 0;
    to = start.row();
  }

  return result;
}
