#include "quickitemdelegatewidget.h"
#include "quickitemmodelroles.h"
#include <QPainter>
#include <QIcon>

using namespace GammaRay;

QuickItemDelegateWidget::QuickItemDelegateWidget(QModelIndex index, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    m_textColor(palette().text().color()),
    m_index(index)
{
  setAutoFillBackground(true);
}

void QuickItemDelegateWidget::paintEvent(QPaintEvent* )
{
  QPainter painter(this);
  int flags = m_index.data(QuickItemModelRole::ItemFlags).value<int>();

  QRect drawRect = rect();

  painter.setPen(m_textColor);

  if (m_index.column() == 0) {
    QVector<QPixmap> icons;
    if ((flags & QuickItemModelRole::OutOfView) && (~flags & QuickItemModelRole::Invisible))
      icons << QIcon::fromTheme("dialog-warning").pixmap(16, 16);
    if (flags & QuickItemModelRole::HasActiveFocus)
      icons << QIcon(":/gammaray/plugins/quickinspector/active-focus.png").pixmap(16, 16);
    if (flags & QuickItemModelRole::HasFocus && ~flags & QuickItemModelRole::HasActiveFocus)
      icons << QIcon(":/gammaray/plugins/quickinspector/focus.png").pixmap(16, 16);

    for (int i = 0; i < icons.size(); i++) {
      painter.drawPixmap(drawRect.topLeft(), icons.at(i));
      drawRect.setTopLeft(drawRect.topLeft() + QPoint(20, 0));
    }
  }

  painter.drawText(drawRect, Qt::AlignVCenter, m_index.data(Qt::DisplayRole).toString());
}

QColor QuickItemDelegateWidget::textColor() const
{
    return m_textColor;
}

void QuickItemDelegateWidget::setTextColor(const QColor& textColor)
{
    m_textColor = textColor;
    update();
}
