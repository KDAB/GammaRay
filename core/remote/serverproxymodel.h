#ifndef GAMMARAY_SERVERPROXYMODEL_H
#define GAMMARAY_SERVERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVector>

namespace GammaRay {

/** Sort/filter proxy model for server-side use to pass through extra roles in itemData(). */
class ServerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ServerProxyModel(QObject* parent = 0);

    void addRole(int role);

    QMap<int, QVariant> itemData(const QModelIndex& index) const override;

private:
    QVector<int> m_extraRoles;
};
}

#endif // GAMMARAY_SERVERPROXYMODEL_H
