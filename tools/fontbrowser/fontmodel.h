
#ifndef FONTMODEL_H
#define FONTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtGui/QFont>

namespace Endoscope
{

class FontModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  FontModel(QObject *parent);

  void updateFonts(const QList<QFont> &fonts);
  QList<QFont> currentFonts() const;

  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& child) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:
  void updateText(const QString &text);

private:
  QList<QFont> m_fonts;
  QString m_text;
};

}

#endif
