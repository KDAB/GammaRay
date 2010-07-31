#ifndef ENDOSCOPE_SCENEMODEL_H
#define ENDOSCOPE_SCENEMODEL_H

#include <qabstractitemmodel.h>

class QGraphicsScene;
class QGraphicsItem;

namespace Endoscope {

class SceneModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    enum Role {
      SceneItemRole = Qt::UserRole + 1
    };
    explicit SceneModel(QObject* parent = 0);
    void setScene( QGraphicsScene* scene );
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  private:
    QList<QGraphicsItem*> topLevelItems() const;
  private:
    QGraphicsScene* m_scene;
};

}

#endif // ENDOSCOPE_SCENEMODEL_H
