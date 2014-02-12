#ifndef METHODSTAB_H
#define METHODSTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_MethodsTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT MethodsTab : public QWidget
{
  Q_OBJECT
public:
  explicit MethodsTab(PropertyWidget *parent);
  virtual ~MethodsTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_MethodsTab *m_ui;
};

}

#endif // METHODSTAB_H
