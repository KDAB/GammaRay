#ifndef OUTBOUNDCONNECTIONSTAB_H
#define OUTBOUNDCONNECTIONSTAB_H

#include "gammaray_ui_export.h"

#include <QWidget>

namespace GammaRay {

class Ui_OutboundConnectionsTab;
class PropertyWidget;

class GAMMARAY_UI_EXPORT OutboundConnectionsTab : public QWidget
{
  Q_OBJECT
public:
  explicit OutboundConnectionsTab(PropertyWidget *parent);
  virtual ~OutboundConnectionsTab();

private slots:
  void setObjectBaseName(const QString &baseName);

private:
  Ui_OutboundConnectionsTab *m_ui;
};

}

#endif // OUTBOUNDCONNECTIONSTABB_H
