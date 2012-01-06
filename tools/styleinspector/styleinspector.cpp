#include "styleinspector.h"
#include "ui_styleinspector.h"

#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>
#include <probeinterface.h>

using namespace GammaRay;

StyleInspector::StyleInspector(ProbeInterface* probe, QWidget* parent): QWidget(parent), ui(new Ui::StyleInspector)
{
  ui->setupUi(this);

  ObjectTypeFilterProxyModel<QStyle> *styleFilter = new ObjectTypeFilterProxyModel<QStyle>(this);
  styleFilter->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(styleFilter);
  ui->styleSelector->setModel(singleColumnProxy);
}

StyleInspector::~StyleInspector()
{
  delete ui;
}

#include "styleinspector.moc"
