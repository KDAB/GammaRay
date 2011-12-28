#ifndef GAMMARAY_LAUNCHERWINDOW_H
#define GAMMARAY_LAUNCHERWINDOW_H

#include <QDialog>

namespace GammaRay {

namespace Ui {
class LauncherWindow;
}

class LauncherWindow : public QDialog
{
  Q_OBJECT
public:
  explicit LauncherWindow(QWidget* parent = 0);
  ~LauncherWindow();

  /// returns the selected PID, if the attach page is active
  QString pid() const;
  /// returns the command and arguments, if the launch page is active
  QStringList launchArguments() const;

private slots:
  void tabChanged();

private:
  Ui::LauncherWindow* ui;
};

}

#endif // GAMMARAY_LAUNCHERWINDOW_H
