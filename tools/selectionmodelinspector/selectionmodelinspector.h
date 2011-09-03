#ifndef ENDOSCOPE_SELECTIONMODELINSPECTOR_H
#define ENDOSCOPE_SELECTIONMODELINSPECTOR_H

#include <qwidget.h>
#include <toolfactory.h>
#include <QItemSelectionModel>

namespace Endoscope {

namespace Ui { class SelectionModelInspector; }

class SelectionModelInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit SelectionModelInspector( ProbeInterface *probe, QWidget *widget = 0 );

  private slots:
    void selectionModelSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::SelectionModelInspector> ui;
};

class SelectionModelInspectorFactory : public QObject, public StandardToolFactory<QItemSelectionModel, SelectionModelInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    virtual inline QString name() const { return tr("Selection Models"); }
};

}

#endif // ENDOSCOPE_SELECTIONMODELINSPECTOR_H
