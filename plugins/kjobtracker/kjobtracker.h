#ifndef GAMMARAY_KJOBTRACKER_H
#define GAMMARAY_KJOBTRACKER_H

#include "include/toolfactory.h"

#include <KJob>
#include <QWidget>

class KJob;
namespace GammaRay {

namespace Ui {
  class KJobTracker;
}

class KJobModel;

class KJobTracker : public QWidget
{
  Q_OBJECT
  public:
    explicit KJobTracker(ProbeInterface *probe, QWidget *parent = 0);
    virtual ~KJobTracker();

  private:
    friend class KJobTrackerFactory;
    QScopedPointer<Ui::KJobTracker> ui;
    static KJobModel* m_jobModel;
};


class KJobTrackerFactory : public QObject, public StandardToolFactory<KJob, KJobTracker>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)

  public:
    explicit KJobTrackerFactory(QObject *parent = 0) : QObject(parent) {}
    inline QString name() const { return tr("KJobs"); }
    void init(ProbeInterface* );
};


}

#endif // GAMMARAY_KJOBTRACKER_H
