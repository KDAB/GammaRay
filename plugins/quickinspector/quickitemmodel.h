#ifndef GAMMARAY_QUICKITEMMODEL_H
#define GAMMARAY_QUICKITEMMODEL_H

#include <core/objectmodelbase.h>

#include <QHash>
#include <QPointer>
#include <QVector>

class QQuickItem;
class QQuickWindow;

namespace GammaRay {

/** QQ2 item tree model. */
class QuickItemModel : public ObjectModelBase<QAbstractItemModel>
{
    Q_OBJECT
public:
    explicit QuickItemModel(QObject *parent = 0);
    ~QuickItemModel();

    void setWindow(QQuickWindow* window);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& child) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;

public slots:
  void objectAdded(QObject *obj);
  void objectRemoved(QObject *obj);
  void itemReparented();

private:
  void clear();
  void populateFromItem(QQuickItem *item);
  QModelIndex indexForItem(QQuickItem *item) const;

  QPointer<QQuickWindow> m_window;

  QHash<QQuickItem*, QQuickItem*> m_childParentMap;
  QHash<QQuickItem*, QVector<QQuickItem*> > m_parentChildMap;
};
}

#endif // GAMMARAY_QUICKITEMMODEL_H
