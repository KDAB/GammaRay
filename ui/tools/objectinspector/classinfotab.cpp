#include "classinfotab.h"
#include "ui_classinfotab.h"
#include "propertywidget.h"

using namespace GammaRay;

ClassInfoTab::ClassInfoTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_ClassInfoTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

ClassInfoTab::~ClassInfoTab()
{
}

void ClassInfoTab::setObjectBaseName(const QString &baseName)
{

}
