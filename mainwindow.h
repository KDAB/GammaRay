#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "ui_mainwindow.h"

namespace Endoscope {

class ObjectPropertyModel;
class SceneModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow( QWidget *parent = 0 );

  private slots:
    void modelSelected(int index);
    void sceneSelected(int index);
    void objectSelected(const QModelIndex &index);

  private:
    Ui::MainWindow ui;
    SceneModel *m_sceneModel;
    ObjectPropertyModel *m_objectPropertyModel;
};

}

#endif // MAINWINDOW_H
