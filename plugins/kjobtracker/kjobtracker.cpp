#include "kjobtracker.h"
#include "kjobmodel.h"
#include "ui_kjobtracker.h"

using namespace GammaRay;

#include "include/probeinterface.h"

#include <QDebug>
#include <QtPlugin>
#include <QSortFilterProxyModel>

using namespace GammaRay;

KJobTracker::KJobTracker(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui(new Ui::KJobTracker)
{
  ui->setupUi(this);

  KJobModel *jobModel = new KJobModel(this);
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)), jobModel, SLOT(objectAdded(QObject*)));

  QSortFilterProxyModel *filter = new QSortFilterProxyModel(this);
  filter->setSourceModel(jobModel);
  ui->searchLine->setProxy(filter);
  ui->jobView->setModel(filter);
}

KJobTracker::~KJobTracker()
{
}


Q_EXPORT_PLUGIN(KJobTrackerFactory)

#include "kjobtracker.moc"
