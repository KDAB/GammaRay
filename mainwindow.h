#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "ui_mainwindow.h"

namespace Endoscope {

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow( QWidget *parent = 0 );

  private slots:
    void modelSelected(int index);

  private:
    Ui::MainWindow ui;
};

}

#endif // MAINWINDOW_H
