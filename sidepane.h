#ifndef GAMMARAY_SIDEPANE_H
#define GAMMARAY_SIDEPANE_H

#include <QListView>

namespace GammaRay {

class SidePane : public QListView
{
  Q_OBJECT
  public:
    explicit SidePane(QWidget *parent = 0);
    virtual ~SidePane();

    virtual QSize sizeHint() const;

  protected:
    virtual void resizeEvent(QResizeEvent *e);
};

}

#endif // GAMMARAY_SIDEPANE_H
