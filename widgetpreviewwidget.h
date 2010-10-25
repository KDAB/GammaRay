#ifndef ENDOSCOPE_WIDGETPREVIEWWIDGET_H
#define ENDOSCOPE_WIDGETPREVIEWWIDGET_H

#include <qwidget.h>
#include <QPointer>

namespace Endoscope {

class WidgetPreviewWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit WidgetPreviewWidget(QWidget* parent = 0);
    void setWidget( QWidget *widget );

  protected:
    virtual void paintEvent(QPaintEvent* event );
    virtual bool eventFilter( QObject *receiver, QEvent *event );

  private:
    QPointer<QWidget> m_widget;
    bool m_grabbingWidget;
};

}

#endif // ENDOSCOPE_WIDGETPREVIEWWIDGET_H
