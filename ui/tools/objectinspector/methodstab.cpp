#include "methodstab.h"
#include "ui_methodstab.h"
#include "propertywidget.h"

using namespace GammaRay;

MethodsTab::MethodsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_MethodsTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

MethodsTab::~MethodsTab()
{
}

void MethodsTab::setObjectBaseName(const QString &baseName)
{

}
