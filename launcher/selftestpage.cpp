#include "selftestpage.h"
#include "ui_selftestpage.h"
#include "probefinder.h"

#include <QStandardItemModel>
#include <qfileinfo.h>
#include <injector/injectorfactory.h>

using namespace GammaRay;

SelfTestPage::SelfTestPage(QWidget* parent): QWidget(parent), ui(new Ui::SelfTestPage), m_resultModel(new QStandardItemModel(this))
{
  ui->setupUi(this);
  ui->resultView->setModel(m_resultModel);
  run();
}

SelfTestPage::~SelfTestPage()
{
  delete ui;
}

void SelfTestPage::run()
{
  m_resultModel->clear();
  testProbe();
  testAvailableInjectors();
  testInjectors();
}

void SelfTestPage::testProbe()
{
  const QString probePath = ProbeFinder::findProbe(QLatin1String("gammaray_probe"));
  if (probePath.isEmpty()) {
    error(tr("No probe found - GammaRay not functional."));
    return;
  }

  QFileInfo fi(probePath);
  if (!fi.exists() || !fi.isFile() || !fi.isReadable()) {
    error(tr("Probe at %1 is invalid - GammaRay not functional."));
    return;
  }

  information(tr("Found valid probe at %1.").arg(probePath));
}

void SelfTestPage::testAvailableInjectors()
{
  const QStringList injectors = InjectorFactory::availableInjectors();
  if (injectors.isEmpty()) {
    error(tr("No injectors available - GammaRay not functional."));
    return;
  }

  information(tr("The following injectors are available: %1").arg(injectors.join(QLatin1String(", "))));
}

void SelfTestPage::testInjectors()
{
  foreach (const QString &injectorType, InjectorFactory::availableInjectors()) {
    AbstractInjector::Ptr injector = InjectorFactory::createInjector(injectorType);
    if (!injector) {
      error(tr("Unable to create instance of injector %1.").arg(injectorType));
      continue;
    }
    if (injector->selfTest()) {
      information(tr("Injector %1 successfully passed its self-test.").arg(injectorType));
    } else {
      error(tr("Injector %1 failed to pass its self-test: %2.").arg(injectorType, injector->errorString()));
    }
  }
}

void SelfTestPage::error(const QString& msg)
{
  QStandardItem *item = new QStandardItem;
  item->setText(msg);
  item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxCritical));
  m_resultModel->appendRow(item);
}

void SelfTestPage::information(const QString& msg)
{
  QStandardItem *item = new QStandardItem;
  item->setText(msg);
  item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
  m_resultModel->appendRow(item);
}

#include "selftestpage.moc"
