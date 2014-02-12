#ifndef ENUMSTAB_H
#define ENUMSTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_EnumsTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT EnumsTab : public QWidget
{
  Q_OBJECT
public:
  explicit EnumsTab(PropertyWidget *parent);
  virtual ~EnumsTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_EnumsTab *m_ui;
};

}

#endif // ENUMSTAB_H
