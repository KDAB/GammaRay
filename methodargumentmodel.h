#ifndef ENDOSCOPE_METHODARGUMENTMODEL_H
#define ENDOSCOPE_METHODARGUMENTMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <qmetaobject.h>
#include <QVector>


namespace Endoscope {

class MethodArgumentModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    MethodArgumentModel(QObject* parent = 0);
    void setMethod( const QMetaMethod &method );

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

  private:
    QMetaMethod m_method;
    QVector<QVariant> m_arguments;
};

}

#endif // ENDOSCOPE_METHODARGUMENTMODEL_H
