#ifndef ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
#define ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H

#include <toolinterface.h>
#include <qwidget.h>
#include <qtextdocument.h>

class QItemSelection;

namespace Endoscope {

class ProbeInterface;
class TextDocumentModel;
class TextDocumentFormatModel;

namespace Ui { class TextDocumentInspector; }

class TextDocumentInspector : public QWidget
{
  Q_OBJECT
  public:
    explicit TextDocumentInspector(ProbeInterface *probe, QWidget* parent = 0);
    virtual ~TextDocumentInspector();

  private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    Ui::TextDocumentInspector* ui;
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

class TextDocumentInspectorInterface : public QObject, public StandardToolInterface<QTextDocument, TextDocumentInspector>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolInterface)
  public:
    inline QString name() const { return tr("Text Documents"); }
};

}

#endif // ENDOSCOPE_TEXTDOCUMENTINSPECTOR_H
