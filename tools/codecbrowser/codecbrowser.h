#ifndef ENDOSCOPE_CODECBROWSER_H
#define ENDOSCOPE_CODECBROWSER_H

#include <qwidget.h>

#include <toolinterface.h>

class QItemSelection;

namespace Endoscope {

class ProbeInterface;
class SelectedCodecsModel;
namespace Ui { class CodecBrowser; }

class CodecBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit CodecBrowser(ProbeInterface* probe, QWidget* parent = 0);

  private slots:
    void updateCodecs(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::CodecBrowser> ui;
    SelectedCodecsModel *m_selectedCodecsModel;
};

class CodecBrowserInterface : public QObject, public StandardToolInterface<QObject, CodecBrowser>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolInterface)
  public:
    virtual inline QString name() const { return tr( "Text Codecs" ); }
};

}

#endif // ENDOSCOPE_CODECBROWSER_H
