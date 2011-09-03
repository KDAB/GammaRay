#ifndef ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
#define ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H

#include <toolfactory.h>
#include <qwidget.h>
#include <qtextdocument.h>

class QItemSelection;

namespace Endoscope {

class TextDocumentModel;
class TextDocumentFormatModel;

namespace Ui { class TextDocumentInspector; }

class TextDocumentInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit TextDocumentInspector(ProbeInterface *probe, QWidget* parent = 0);

  private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::TextDocumentInspector> ui;
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

class TextDocumentInspectorFactory : public QObject, public StandardToolFactory<QTextDocument, TextDocumentInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    inline QString name() const { return tr("Text Documents"); }
};

}

#endif // ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
