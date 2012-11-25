#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include <QObject>
#include <QPointer>

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
  static bool isConnected();
  static quint16 defaultPort();

signals:
  void disconnected();
  // ### temporary
  void messageReceived(const GammaRay::Message &msg);

protected:
  Endpoint(QObject* parent = 0);
  /// takes ownership
  void setDevice(QIODevice* device);

private slots:
  void readyRead();
  void connectionClosed();

private:
  static Endpoint *s_instance;
  QPointer<QIODevice> m_socket;
  QScopedPointer<QDataStream> m_stream;
};
}

#endif // GAMMARAY_ENDPOINT_H
