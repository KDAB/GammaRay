#include "launcherwindow.h"
#include "ui_launcherwindow.h"

#include <QPushButton>
#include <QSettings>

using namespace GammaRay;

LauncherWindow::LauncherWindow(QWidget* parent): QDialog(parent), ui( new Ui::LauncherWindow )
{
  ui->setupUi(this);
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged()));
  connect(ui->attachPage, SIGNAL(updateButtonState()), SLOT(tabChanged()));
  connect(ui->launchPage, SIGNAL(updateButtonState()), SLOT(tabChanged()));
  connect(ui->attachPage, SIGNAL(activate()), ui->buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

  QSettings settings;
  ui->tabWidget->setCurrentIndex(settings.value(QLatin1String("Launcher/TabIndex")).toInt());
}

LauncherWindow::~LauncherWindow()
{
  delete ui;
}

QString LauncherWindow::pid() const
{
  if (ui->tabWidget->currentWidget() == ui->attachPage)
    return ui->attachPage->pid();
  return QString();
}

QStringList LauncherWindow::launchArguments() const
{
  if (ui->tabWidget->currentWidget() == ui->launchPage)
    return ui->launchPage->launchArguments();
  return QStringList();
}

void LauncherWindow::tabChanged()
{
  if (ui->tabWidget->currentWidget() == ui->attachPage) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->attachPage->isValid());
  } else if (ui->tabWidget->currentWidget() == ui->launchPage) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Launch"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->launchPage->isValid());
  } else {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
  }
}

void LauncherWindow::accept()
{
  QSettings settings;
  settings.setValue(QLatin1String("Launcher/TabIndex"), ui->tabWidget->currentIndex());

  ui->launchPage->writeSettings();

  QDialog::accept();
}


#include "launcherwindow.moc"
