#ifndef ENDOSCOPE_SINGLECOLUMNOBJECTPROXYMODEL_H
#define ENDOSCOPE_SINGLECOLUMNOBJECTPROXYMODEL_H

#include "kde/kidentityproxymodel.h"

namespace Endoscope {

class SingleColumnObjectProxyModel : public KIdentityProxyModel
{
  Q_OBJECT
  public:
    explicit SingleColumnObjectProxyModel(QObject* parent = 0);
    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
};

}

#endif // ENDOSCOPE_SINGLECOLUMNOBJECTPROXYMODEL_H
