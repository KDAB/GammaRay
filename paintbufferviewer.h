#ifndef GAMMARAY_PAINTBUFFERVIEWER_H
#define GAMMARAY_PAINTBUFFERVIEWER_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS

#include <qwidget.h>
#include <private/qpaintbuffer_p.h>

namespace GammaRay {

class PaintBufferModel;

namespace Ui { class PaintBufferViewer; }

/**
 * A widget to look at the command list in a QPaintBuffer.
 */
class PaintBufferViewer : public QWidget
{
  Q_OBJECT
  public:
    explicit PaintBufferViewer(QWidget* parent = 0);
    virtual ~PaintBufferViewer();

    void setPaintBuffer(const QPaintBuffer &buffer);

  private slots:
    void commandSelected();

  private:
    QScopedPointer<Ui::PaintBufferViewer> ui;
    QPaintBuffer m_buffer;
    PaintBufferModel *m_bufferModel;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERVIEWER_H
