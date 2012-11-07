#ifndef GAMMARAY_MODELUTILS_H
#define GAMMARAY_MODELUTILS_H

#include <QModelIndex>

namespace GammaRay {

namespace ModelUtils {

  /**
  * Return a list of items that are accepted by an acceptor function
  * This class extends the functionality provided in QAbstractItemModel::match(...)
  *
  * If (accept(v)) for an item v in the model returns true, it will be appended
  * to the result list
  *
  * @param accept Function in the form 'bool f(const QVariant&)'
  *
  * @see QAbstractItemModel::match(...)
  */
  QModelIndexList match(const QAbstractItemModel* model, const QModelIndex& start,
                              int role, bool (*accept)(const QVariant&),
                              int hits = 1, Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchWrap));

};

}

#endif // GAMMARAY_MODELUTILS_H
