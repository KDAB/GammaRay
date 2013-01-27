#include "connectpage.h"
#include "ui_connectpage.h"

#include <network/endpoint.h>

#include <QProcess>
#include <QSettings>

using namespace GammaRay;

ConnectPage::ConnectPage(QWidget* parent): QWidget(parent), ui(new Ui::ConnectPage)
{
  ui->setupUi(this);

  connect(ui->host, SIGNAL(textChanged(QString)), SIGNAL(updateButtonState()));
  connect(ui->port, SIGNAL(valueChanged(int)), SIGNAL(updateButtonState()));

  // TODO add discovery model

  QSettings settings;
  ui->host->setText(settings.value("Connect/Host", QString()).toString());
  ui->port->setValue(settings.value("Connect/Port", Endpoint::defaultPort()).toInt());
}

ConnectPage::~ConnectPage()
{
}

bool ConnectPage::isValid() const
{
  return !ui->host->text().isEmpty();
}

void ConnectPage::launchClient()
{
  QStringList args;
  args.push_back(ui->host->text());
  args.push_back(QString::number(ui->port->value()));
  // TODO be more clever in finding the executable
  QProcess::startDetached("gammaray-client", args);
}

void ConnectPage::writeSettings()
{
  QSettings settings;
  settings.setValue("Connect/Host", ui->host->text());
  settings.setValue("Connect/Port", ui->port->value());
}

#include "connectpage.moc"
