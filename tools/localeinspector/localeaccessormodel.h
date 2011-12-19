
#ifndef GAMMARAY_LOCALEACCESSORMODEL_H
#define GAMMARAY_LOCALEACCESSORMODEL_H

#include <QAbstractTableModel>
#include <QVector>

namespace GammaRay
{

struct LocaleDataAccessor;

class LocaleAccessorModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_ENUMS(CustomRoles)
public:
  enum CustomRoles
  {
    AccessorRole = Qt::UserRole + 1
  };
  explicit LocaleAccessorModel(QObject* parent = 0);

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
  QVector<LocaleDataAccessor*> enabledAccessors;
};

}

#endif
