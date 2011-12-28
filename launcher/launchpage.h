#ifndef GAMMARAY_LAUNCHPAGE_H
#define GAMMARAY_LAUNCHPAGE_H

#include <qwidget.h>

class QStringListModel;

namespace GammaRay {

namespace Ui {
class LaunchPage;
}

class LaunchPage : public QWidget
{
  Q_OBJECT
public:
  explicit LaunchPage(QWidget* parent = 0);
  ~LaunchPage();

  QStringList launchArguments() const;
  bool isValid();

  void writeSettings();

signals:
  void updateButtonState();

private slots:
  void showFileDialog();
  void addArgument();
  void removeArgument();
  void updateArgumentButtons();

private:
  Ui::LaunchPage* ui;
  QStringListModel* m_argsModel;
};

}

#endif // GAMMARAY_LAUNCHPAGE_H
