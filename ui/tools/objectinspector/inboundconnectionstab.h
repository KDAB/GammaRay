#ifndef INBOUNDCONNECTIONS_H
#define INBOUNDCONNECTIONS_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_InboundConnectionsTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT InboundConnectionsTab : public QWidget
{
  Q_OBJECT
public:
  explicit InboundConnectionsTab(PropertyWidget *parent);
  virtual ~InboundConnectionsTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_InboundConnectionsTab *m_ui;
};

}

#endif // INBOUNDCONNECTIONS_H
