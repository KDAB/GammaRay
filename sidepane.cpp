#include "sidepane.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace GammaRay;

class Delegate : public QStyledItemDelegate
{
public:
  explicit Delegate(QObject* parent = 0)
    : QStyledItemDelegate(parent)
  {
  }

  virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
  {
    static const int heightMargin = 10;

    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(size.height() + heightMargin);
    return size;
  }
};

SidePane::SidePane(QWidget* parent)
  : QListView(parent)
{
  viewport()->setAutoFillBackground(false);

  setItemDelegate(new Delegate(this));
}

SidePane::~SidePane()
{
}

QSize SidePane::sizeHint() const
{
  static const int widthMargin = 0;

  if (!model())
    return QSize(0, 0);

  const int width = sizeHintForColumn(0) + widthMargin;
  const int height = QListView::sizeHint().height();

  return QSize(width, height);
}

void SidePane::resizeEvent(QResizeEvent* e)
{
  QListView::resizeEvent(e);
}

#include "sidepane.moc"
