
#ifndef CODECMODEL_H
#define CODECMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QTextCodec>
#include <QtCore/QStringList>

namespace Endoscope
{

class AllCodecsModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  AllCodecsModel(QObject *parent);

  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& child) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
};

class SelectedCodecsModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  SelectedCodecsModel(QObject *parent);

  void setCodecs(const QStringList &codecs);
  QStringList currentCodecs() const;

  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex& child) const;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
  void updateText(const QString &text);

private:
  QStringList m_codecs;
  QString m_text;
};

}

#endif
