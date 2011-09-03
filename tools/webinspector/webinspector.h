#ifndef ENDOSCOPE_WEBINSPECTOR_H
#define ENDOSCOPE_WEBINSPECTOR_H

#include <toolfactory.h>
#include <qwidget.h>
#include <qwebpage.h>

namespace Endoscope {

namespace Ui { class WebInspector; }

class WebInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit WebInspector(Endoscope::ProbeInterface* probe, QWidget* parent = 0);

  private slots:
    void webPageSelected(int index);

  private:
    QScopedPointer<Ui::WebInspector> ui;
};

class WebInspectorFactory : public QObject, public StandardToolFactory<QWebPage, WebInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Web Pages"); }
};

}

#endif // ENDOSCOPE_WEBINSPECTOR_H
