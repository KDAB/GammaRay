#include "selftestpage.h"
#include "ui_selftestpage.h"
#include "probefinder.h"

#include <QStandardItemModel>
#include <qfileinfo.h>

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
