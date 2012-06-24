#include "kjobtracker.h"
#include "kjobmodel.h"
#include "ui_kjobtracker.h"

using namespace GammaRay;

#include "include/probeinterface.h"

#include <QDebug>
#include <QtPlugin>
#include <QSortFilterProxyModel>

using namespace GammaRay;

KJobModel* KJobTracker::m_jobModel = 0;

KJobTracker::KJobTracker(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui(new Ui::KJobTracker)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  QSortFilterProxyModel *filter = new QSortFilterProxyModel(this);
  filter->setSourceModel(m_jobModel);
  ui->searchLine->setProxy(filter);
  ui->jobView->setModel(filter);
}

KJobTracker::~KJobTracker()
{
}


void KJobTrackerFactory::init(ProbeInterface *probe)
{
  GammaRay::StandardToolFactory<KJob, GammaRay::KJobTracker>::init(probe);
  if (!KJobTracker::m_jobModel) {
    KJobTracker::m_jobModel = new KJobModel(this);
    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), KJobTracker::m_jobModel, SLOT(objectAdded(QObject*)));
  }
}

Q_EXPORT_PLUGIN(KJobTrackerFactory)

#include "kjobtracker.moc"
