#ifndef GAMMARAY_PAINTBUFFERMODEL_H
#define GAMMARAY_PAINTBUFFERMODEL_H

#include "config-gammaray.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <qabstractitemmodel.h>

#include <private/qpaintbuffer_p.h>

class QPaintBuffer;

namespace GammaRay {


/**
 * Model that shows commands stored in a QPaintBuffer.
 */
class PaintBufferModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit PaintBufferModel(QObject* parent = 0);

    void setPaintBuffer(const QPaintBuffer &buffer);

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private:
    QPaintBuffer m_buffer;
    QPaintBufferPrivate *m_privateBuffer;
};

}

#endif

#endif // GAMMARAY_PAINTBUFFERMODEL_H
