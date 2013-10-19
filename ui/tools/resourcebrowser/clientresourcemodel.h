#ifndef GAMMARAY_CLIENTRESOURCEMODEL_H
#define GAMMARAY_CLIENTRESOURCEMODEL_H

#include "kde/krecursivefilterproxymodel.h"
#include <QFileIconProvider>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0 ,0)
#include <QMimeDatabase>
#endif

namespace GammaRay {

/**
 * Adds file icons for the resource model.
 * This can't be done server-side since the icon stuff might not exist in a pure QtCore application.
 */
class ClientResourceModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT
public:
  explicit ClientResourceModel(QObject *parent = 0);
  ~ClientResourceModel();

  /*override*/ QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
  QFileIconProvider m_iconProvider;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  QMimeDatabase m_mimeDb;
#endif
};
}

#endif // GAMMARAY_CLIENTRESOURCEMODEL_H
