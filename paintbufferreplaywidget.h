#ifndef GAMMARAY_PAINTBUFFERREPLAYWIDGET_H
#define GAMMARAY_PAINTBUFFERREPLAYWIDGET_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <qwidget.h>

#include <private/qpaintbuffer_p.h>

namespace GammaRay {

/**
 * A widget painting a certain sub-set of a QPaintBuffer.
 */
class PaintBufferReplayWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit PaintBufferReplayWidget(QWidget* parent = 0);

    void setPaintBuffer(const QPaintBuffer &buffer);
    void setEndCommandIndex(int index);

    virtual QSize sizeHint() const;

  protected:
    virtual void paintEvent(QPaintEvent* event);

  private:
    QPaintBuffer m_buffer;
    int m_endCommandIndex;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERREPLAYWIDGET_H
