#ifndef GAMMARAY_MIMETYPESMODEL_H
#define GAMMARAY_MIMETYPESMODEL_H

#include <QStandardItemModel>
#include <QMimeDatabase>

namespace GammaRay {

class MimeTypesModel : public QStandardItemModel
{
  Q_OBJECT
public:
  explicit MimeTypesModel(QObject* parent = 0);
  ~MimeTypesModel();

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
  void fillModel();
  QVector<QStandardItem*> itemsForType(const QString& mimeTypeName);
  void makeItemsForType(const QString &mimeTypeName);
  static QList<QStandardItem*> makeRowForType(const QMimeType &mt);
  QSet<QString> normalizedMimeTypeNames(const QStringList &typeNames) const;

  QHash<QString, QVector<QStandardItem*> > m_mimeTypeNodes;
  QMimeDatabase m_db;
};

}

#endif // GAMMARAY_MIMETYPESMODEL_H
