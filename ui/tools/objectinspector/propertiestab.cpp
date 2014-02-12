#include "propertiestab.h"
#include "ui_propertiestab.h"
#include "propertywidget.h"

using namespace GammaRay;

PropertiesTab::PropertiesTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_PropertiesTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString));
}

PropertiesTab::~PropertiesTab()
{
}

void PropertiesTab::setObjectBaseName(const QString &baseName)
{

}
