#ifndef ENDOSCOPE_SCENEMODEL_H
#define ENDOSCOPE_SCENEMODEL_H

#include <qabstractitemmodel.h>

class QGraphicsScene;

namespace Endoscope {

class SceneModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    SceneModel(QObject* parent = 0);
    void setScene( QGraphicsScene* scene );
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  private:
    QGraphicsScene* m_scene;
};

}

#endif // ENDOSCOPE_SCENEMODEL_H
