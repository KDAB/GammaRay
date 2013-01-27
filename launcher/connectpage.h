#ifndef GAMMARAY_CONNECTPAGE_H
#define GAMMARAY_CONNECTPAGE_H

#include <QWidget>

namespace GammaRay {

namespace Ui {
class ConnectPage;
}

/** UI for connecting to a running GammaRay instance. */
class ConnectPage : public QWidget
{
  Q_OBJECT
public:
  explicit ConnectPage(QWidget* parent = 0);
  ~ConnectPage();

  bool isValid() const;
  void writeSettings();

public slots:
  void launchClient();

signals:
  void updateButtonState();

private slots:
  void instanceSelected();

private:
  QScopedPointer<Ui::ConnectPage> ui;

};
}

#endif // GAMMARAY_CONNECTPAGE_H
