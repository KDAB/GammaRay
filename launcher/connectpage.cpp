#include "connectpage.h"
#include "ui_connectpage.h"

#include <network/endpoint.h>

#include <QProcess>

using namespace GammaRay;

ConnectPage::ConnectPage(QWidget* parent): QWidget(parent), ui(new Ui::ConnectPage)
{
  ui->setupUi(this);

  ui->port->setValue(Endpoint::defaultPort());
  connect(ui->host, SIGNAL(textChanged(QString)), SIGNAL(updateButtonState()));
  connect(ui->port, SIGNAL(valueChanged(int)), SIGNAL(updateButtonState()));

  // TODO add discovery model
  // TODO read settings
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

#include "connectpage.moc"