#ifndef GAMMARAY_OBJECTVISUALIZERMODEL_H
#define GAMMARAY_OBJECTVISUALIZERMODEL_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
#include <QSortFilterProxyModel>
typedef QSortFilterProxyModel QIdentityProxyModel;
#else
#include <QIdentityProxyModel>
#endif

#include "include/objectmodel.h"

namespace GammaRay {

/** Augment the regular object tree by some information needed for the visualization
 * on the client side.
 */
class ObjectVisualizerModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    enum Role {
      ObjectId = ObjectModel::UserRole,
      ObjectDisplayName,
      ClassName
    };

    explicit ObjectVisualizerModel(QObject *parent);
    ~ObjectVisualizerModel();

    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
    QMap<int, QVariant> itemData(const QModelIndex& index) const;
};
}

#endif // GAMMARAY_OBJECTVISUALIZERMODEL_H
