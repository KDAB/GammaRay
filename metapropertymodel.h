#ifndef GAMMARAY_METAPROPERTYMODEL_H
#define GAMMARAY_METAPROPERTYMODEL_H

#include <QtCore/QAbstractTableModel>

namespace GammaRay {

class MetaObject;

/** Model showing non-QObject object properties.
 * @todo needs better name, but ObjectPropertyModel is already in use...
 * @todo maybe it's a good to merge those to anyway?
 */
class MetaPropertyModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit MetaPropertyModel( QObject* parent = 0 );

  /** Sets the object that should be represented by this model. */
  void setObject( void *object, const QString &typeName );
  void setObject( QObject *object );

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex& index) const;

private:
  MetaObject* m_metaObject;
  void* m_object;
};

}

#endif // GAMMARAY_METAPROPERTYMODEL_H
