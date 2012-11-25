#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include <QtCore/QObject>

class QIODevice;
class QDataStream;

namespace GammaRay {

class Message;

/** Network protocol endpoint. */
class Endpoint : public QObject
{
  Q_OBJECT
public:
  ~Endpoint();

  static QDataStream& stream();
  static quint16 defaultPort();

signals:
  // ### temporary
  void messageReceived(const GammaRay::Message &msg);

protected:
  Endpoint(QObject* parent = 0);
  void setDevice(QIODevice* device);

private slots:
  void readyRead();

private:
  static Endpoint *s_instance;
  QIODevice *m_socket;
  QScopedPointer<QDataStream> m_stream;
};
}

#endif // GAMMARAY_ENDPOINT_H
