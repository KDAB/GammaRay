#ifndef GAMMARAY_PIXELMETRICMODEL_H
#define GAMMARAY_PIXELMETRICMODEL_H

#include <qabstractitemmodel.h>

class QStyle;
namespace GammaRay {

/**
 * Lists all pixel metric values of a given QStyle.
 */
class PixelMetricModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit PixelMetricModel(QObject* parent = 0);

  void setStyle(QStyle* style);

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
  QStyle* m_style;
};

}

#endif // GAMMARAY_PIXELMETRICMODEL_H
