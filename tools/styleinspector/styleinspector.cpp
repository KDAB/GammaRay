#include "styleinspector.h"
#include "ui_styleinspector.h"

#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>
#include <probeinterface.h>
#include "pixelmetricmodel.h"

using namespace GammaRay;

StyleInspector::StyleInspector(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui(new Ui::StyleInspector),
  m_pixelMetricModel(new PixelMetricModel(this))
{
  ui->setupUi(this);

  ObjectTypeFilterProxyModel<QStyle> *styleFilter = new ObjectTypeFilterProxyModel<QStyle>(this);
  styleFilter->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(styleFilter);
  ui->styleSelector->setModel(singleColumnProxy);
  connect(ui->styleSelector, SIGNAL(activated(int)), SLOT(styleSelected(int)));

  ui->pixelMetricView->setModel(m_pixelMetricModel);

  if (ui->styleSelector->count())
    styleSelected(0);
}

StyleInspector::~StyleInspector()
{
  delete ui;
}

void StyleInspector::styleSelected(int index)
{
  QObject *obj = ui->styleSelector->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QStyle *style = qobject_cast<QStyle*>(obj);
  if (style) {
    m_pixelMetricModel->setStyle(style);
  }
}

#include "styleinspector.moc"
