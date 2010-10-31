#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "ui_mainwindow.h"

namespace Endoscope {

class ModelCellModel;
class SceneModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow( QWidget *parent = 0 );

  private slots:
    void objectSelected(const QModelIndex &index);
    void widgetSelected( const QModelIndex &index );
    void sceneItemSelected( const QModelIndex &index );
    void modelSelected(const QModelIndex &index);
    void modelCellSelected(const QModelIndex &index);
    void sceneSelected(int index);
    void scriptEngineSelected(int index);
    void webPageSelected(int index);

  private:
    Ui::MainWindow ui;
    SceneModel *m_sceneModel;
    ModelCellModel *m_cellModel;
};

}

#endif // MAINWINDOW_H
