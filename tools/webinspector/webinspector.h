#ifndef ENDOSCOPE_WEBINSPECTOR_H
#define ENDOSCOPE_WEBINSPECTOR_H

#include <toolinterface.h>
#include <qwidget.h>
#include <qwebpage.h>

namespace Endoscope {

class ProbeInterface;
namespace Ui { class WebInspector; }

class WebInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit WebInspector(Endoscope::ProbeInterface* probe, QWidget* parent = 0);
    virtual ~WebInspector();

  private slots:
    void webPageSelected(int index);

  private:
    Ui::WebInspector* ui;
};

class WebInspectorInterface : public QObject, public StandardToolInterface<QWebPage, WebInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolInterface)
  public:
    inline QString name() const { return tr("Web Pages"); }
};

}

#endif // ENDOSCOPE_WEBINSPECTOR_H
