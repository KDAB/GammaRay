#ifndef ENDOSCOPE_WIDGETPREVIEWWIDGET_H
#define ENDOSCOPE_WIDGETPREVIEWWIDGET_H

#include <qwidget.h>


namespace Endoscope {

class WidgetPreviewWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit WidgetPreviewWidget(QWidget* parent = 0);
    void setWidget( QWidget *widget );

  protected:
    virtual void paintEvent(QPaintEvent* event );

  private:
    QWeakPointer<QWidget> m_widget;
};

}

#endif // ENDOSCOPE_WIDGETPREVIEWWIDGET_H
