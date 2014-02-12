#ifndef CLASSINFOTAB_H
#define CLASSINFOTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_ClassInfoTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT ClassInfoTab : public QWidget
{
  Q_OBJECT
public:
  explicit ClassInfoTab(PropertyWidget *parent);
  virtual ~ClassInfoTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_ClassInfoTab *m_ui;
};

}

#endif // CLASSINFOTAB_H
