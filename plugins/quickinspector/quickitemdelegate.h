#ifndef QUICKITEMDELEGATE_H
#define QUICKITEMDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>

class QTreeView;
namespace GammaRay {

class QuickItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    QuickItemDelegate(QTreeView* view);

public Q_SLOTS:
    void setTextColor(const QVariant& textColor);

protected:
    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  QHash<QModelIndex, QColor> m_colors;
  QTreeView *m_view;
};

}

#endif // QUICKITEMDELEGATE_H
