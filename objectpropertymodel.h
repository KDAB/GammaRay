#ifndef ENDOSCOPE_OBJECTPROPERTYMODEL_H
#define ENDOSCOPE_OBJECTPROPERTYMODEL_H

#include <qabstractitemmodel.h>
#include <QWeakPointer>

class QTimer;

namespace Endoscope {

class ObjectPropertyModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    ObjectPropertyModel(QObject* parent = 0);
    void setObject( QObject *object );

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  protected:
    QWeakPointer<QObject> m_obj;

  private slots:
    void slotReset() { reset(); }
    void updateAll();
    void doEmitChanged();

  private:
    QTimer *m_updateTimer;
};

}

#endif // ENDOSCOPE_OBJECTPROPERTYMODEL_H
