#ifndef PROPERTIESTAB_H
#define PROPERTIESTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_PropertiesTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT PropertiesTab : public QWidget
{
  Q_OBJECT
public:
  explicit PropertiesTab(PropertyWidget *parent);
  virtual ~PropertiesTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_PropertiesTab *m_ui;
};

}

#endif // PROPERTIESTAB_H
