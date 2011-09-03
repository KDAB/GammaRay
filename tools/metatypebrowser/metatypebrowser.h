#ifndef ENDOSCOPE_METATYPEBROWSER_H
#define ENDOSCOPE_METATYPEBROWSER_H

#include <qwidget.h>
#include <toolfactory.h>

namespace Endoscope {

namespace Ui { class MetaTypeBrowser; }

class MetaTypeBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit MetaTypeBrowser( ProbeInterface *probe, QWidget *parent = 0 );

  private:
    QScopedPointer<Ui::MetaTypeBrowser> ui;
};

class MetaTypeBrowserFactory : public QObject, public StandardToolFactory<QObject, MetaTypeBrowser>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Meta Types"); }
};

}

#endif // ENDOSCOPE_METATYPEBROWSER_H
