#ifndef GAMMARAY_STANDARDICONMODEL_H
#define GAMMARAY_STANDARDICONMODEL_H

#include <qabstractitemmodel.h>
#include <qstyle.h>

namespace GammaRay {

/**
 * Lists all standard icons of a style.
 */
class StandardIconModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit StandardIconModel(QObject* parent = 0);

  void setStyle(QStyle* style);

  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
  QVariant dataForStandardIcon(QStyle::StandardPixmap stdPix, const QString &name, int column, int role) const;

private:
  QStyle* m_style;
};

}

#endif // GAMMARAY_STANDARDICONMODEL_H
