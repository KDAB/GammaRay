#include "launcherwindow.h"
#include "ui_launcherwindow.h"

#include <QPushButton>

using namespace GammaRay;

LauncherWindow::LauncherWindow(QWidget* parent): QDialog(parent), ui( new Ui::LauncherWindow )
{
  ui->setupUi(this);
  connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged()));
  connect(ui->attachPage, SIGNAL(updateButtonState()), SLOT(tabChanged()));
  connect(ui->attachPage, SIGNAL(activate()), ui->buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));
}

LauncherWindow::~LauncherWindow()
{
  delete ui;
}

QString LauncherWindow::pid() const
{
  return ui->attachPage->pid();
}

QStringList LauncherWindow::launchArguments() const
{
  return QStringList();
}

void LauncherWindow::tabChanged()
{
  if (ui->tabWidget->currentWidget() == ui->attachPage) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->attachPage->isValid());
  } else if (ui->tabWidget->currentWidget() == ui->launchPage) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Launch"));
  }
}

#include "launcherwindow.moc"
