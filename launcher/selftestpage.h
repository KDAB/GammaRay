#ifndef GAMMARAY_SELFTESTPAGE_H
#define GAMMARAY_SELFTESTPAGE_H

#include <qwidget.h>

class QStandardItemModel;
namespace GammaRay {

namespace Ui {
class SelfTestPage;
}

class SelfTestPage : public QWidget
{
  Q_OBJECT
public:
  explicit SelfTestPage(QWidget* parent = 0);
  ~SelfTestPage();

public slots:
  void run();

private:
  void testProbe();
  void testAvailableInjectors();

  void error(const QString &msg);
  void information(const QString &msg);

private:
  Ui::SelfTestPage* ui;
  QStandardItemModel *m_resultModel;
};

}

#endif // GAMMARAY_SELFTESTPAGE_H
