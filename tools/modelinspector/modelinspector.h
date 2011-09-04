#ifndef ENDOSCOPE_MODELINSPECTOR_H
#define ENDOSCOPE_MODELINSPECTOR_H

#include <qwidget.h>
#include <QtCore/QAbstractItemModel>
#include <toolfactory.h>

namespace Endoscope {

class ModelCellModel;

namespace Ui { class ModelInspector; }

class ModelInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit ModelInspector( ProbeInterface *probe, QWidget *parent = 0 );

  private slots:
    void modelSelected(const QModelIndex &index);
    void modelCellSelected(const QModelIndex &index);

  private:
    QScopedPointer<Ui::ModelInspector> ui;
    ModelCellModel *m_cellModel;
};

class ModelInspectorFactory : public QObject, public StandardToolFactory<QAbstractItemModel, ModelInspector>
{
  Q_OBJECT
  Q_INTERFACES( Endoscope::ToolFactory )
  public:
    inline QString name() const { return tr( "Models" ); }
};

}

#endif // ENDOSCOPE_MODELINSPECTOR_H
