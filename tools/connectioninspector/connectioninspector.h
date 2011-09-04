#ifndef ENDOSCOPE_CONNECTIONINSPECTOR_H
#define ENDOSCOPE_CONNECTIONINSPECTOR_H

#include <qwidget.h>

#include <toolfactory.h>

namespace Endoscope {

namespace Ui { class ConnectionInspector; }

class ConnectionInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit ConnectionInspector( ProbeInterface *probe, QWidget *parent = 0 );

  private:
    QScopedPointer<Ui::ConnectionInspector> ui;
};

class ConnectionInspectorFactory : public QObject, public StandardToolFactory<QObject, ConnectionInspector>
{
  Q_OBJECT
  Q_INTERFACES( Endoscope::ToolFactory )
  public:
    inline QString name() const { return tr( "Connections" ); }
};

}

#endif // ENDOSCOPE_CONNECTIONINSPECTOR_H
