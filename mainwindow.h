#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "ui_mainwindow.h"

namespace Endoscope {

class ModelCellModel;
class SceneModel;
class StateModel;
class TransitionModel;
class FontModel;
class SelectedCodecsModel;
class TextDocumentModel;
class TextDocumentFormatModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    explicit MainWindow( QWidget *parent = 0 );

  private slots:
    void objectSelected(const QModelIndex &index);
    void widgetSelected( const QModelIndex &index );
    void widgetSelected( QWidget* widget );
    void sceneItemSelected( const QModelIndex &index );
    void sceneItemSelected( QGraphicsItem* item );
    void modelSelected(const QModelIndex &index);
    void modelCellSelected(const QModelIndex &index);
    void sceneSelected(int index);
    void resourceSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void stateMachineSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void stateSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void selectionModelSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void updateFonts(const QItemSelection &selected, const QItemSelection &deselected);
    void updateCodecs(const QItemSelection &selected, const QItemSelection &deselected);
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void about();
    void toolSelected();

  private:
    Ui::MainWindow ui;
    QComboBox *m_toolSelector;
    SceneModel *m_sceneModel;
    StateModel *m_stateModel;
    ModelCellModel *m_cellModel;
    TransitionModel *m_transitionModel;
    FontModel *m_selectedFontModel;
    SelectedCodecsModel *m_selectedCodecsModel;
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

}

#endif // MAINWINDOW_H
