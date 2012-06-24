#ifndef GAMMARAY_KJOBMODEL_H
#define GAMMARAY_KJOBMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class KJob;

namespace GammaRay {

class KJobModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit KJobModel(QObject* parent = 0);
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  private slots:
    void objectAdded(QObject *obj);
    void jobResult(KJob* job);
    void jobFinished(KJob* obj);
    void jobInfo(KJob* job, const QString &plainMessage);

  private:
    int indexOfJob(QObject *obj) const;

    struct KJobInfo {
      KJob *job;
      QString name;
      QString type;
      QString statusText;
      enum {
        Running,
        Finished,
        Error,
        Killed
      } state;
    };
    QVector<KJobInfo> m_data;
};

}

#endif // GAMMARAY_KJOBMODEL_H
