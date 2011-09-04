#ifndef ENDOSCOPE_RESOURCEBROWSER_H
#define ENDOSCOPE_RESOURCEBROWSER_H

#include <qwidget.h>
#include <toolfactory.h>

class QItemSelection;

namespace Endoscope {

namespace Ui { class ResourceBrowser; }

class ResourceBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit ResourceBrowser( ProbeInterface* probe, QWidget *parent = 0 );

  private slots:
    void resourceSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::ResourceBrowser> ui;
};

class ResourceBrowserFactory : public QObject, public StandardToolFactory<QObject, ResourceBrowser>
{
  Q_OBJECT
  Q_INTERFACES( Endoscope::ToolFactory )
  public:
    inline QString name() const { return tr( "Resources" ); }
};

}

#endif // ENDOSCOPE_RESOURCEBROWSER_H
