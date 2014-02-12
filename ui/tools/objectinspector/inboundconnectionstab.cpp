#include "inboundconnectionstab.h"
#include "ui_inboundconnectionstab.h"
#include "propertywidget.h"

using namespace GammaRay;

InboundConnectionsTab::InboundConnectionsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_InboundConnectionsTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

InboundConnectionsTab::~InboundConnectionsTab()
{
}

void InboundConnectionsTab::setObjectBaseName(const QString &baseName)
{

}
