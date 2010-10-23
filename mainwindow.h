#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "ui_mainwindow.h"

namespace Endoscope {

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
    void modelSelected(int index);
    void sceneSelected(int index);
    void scriptEngineSelected(int index);

  private:
    Ui::MainWindow ui;
    SceneModel *m_sceneModel;
};

}

#endif // MAINWINDOW_H
