#include "enumstab.h"
#include "ui_enumstab.h"
#include "propertywidget.h"

using namespace GammaRay;

EnumsTab::EnumsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_EnumsTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

EnumsTab::~EnumsTab()
{
}

void EnumsTab::setObjectBaseName(const QString &baseName)
{

}
