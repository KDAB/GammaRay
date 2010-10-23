#ifndef ENDOSCOPE_MODELTESTER_H
#define ENDOSCOPE_MODELTESTER_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QWeakPointer>

class QAbstractItemModel;
class ModelTest;

namespace Endoscope {

class ModelTester : public QObject
{
  Q_OBJECT
  public:
    explicit ModelTester(QObject* parent = 0);
    
    void failure( QAbstractItemModel* model, int line, const char *message );
    
  public slots:
    void objectAdded( const QWeakPointer<QObject> &obj );
    
  private slots:
    void modelDestroyed( QObject *model );
    
  private:
    struct ModelTestResult;
    QHash<QAbstractItemModel*, ModelTestResult*> m_modelTestMap;
};

}

#endif // ENDOSCOPE_MODELTESTER_H
