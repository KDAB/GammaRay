#ifndef GAMMARAY_WEBVIEWMODEL_H
#define GAMMARAY_WEBVIEWMODEL_H

#include "include/objecttypefilterproxymodel.h"

namespace GammaRay {

class WebViewModel : public ObjectFilterProxyModelBase
{
    Q_OBJECT
public:
    explicit WebViewModel(QObject *parent = 0);
    ~WebViewModel();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool filterAcceptsObject(QObject* object) const;
};

}

#endif // GAMMARAY_WEBVIEWMODEL_H
