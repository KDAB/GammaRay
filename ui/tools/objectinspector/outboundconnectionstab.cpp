#include "outboundconnectionstab.h"
#include "ui_outboundconnectionstab.h"
#include "propertywidget.h"

using namespace GammaRay;

OutboundConnectionsTab::OutboundConnectionsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_OutboundConnectionsTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

OutboundConnectionsTab::~OutboundConnectionsTab()
{
}

void OutboundConnectionsTab::setObjectBaseName(const QString &baseName)
{

}
