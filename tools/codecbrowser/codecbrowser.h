#ifndef ENDOSCOPE_CODECBROWSER_H
#define ENDOSCOPE_CODECBROWSER_H

#include <qwidget.h>

#include <toolfactory.h>

class QItemSelection;

namespace Endoscope {

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

class CodecBrowserFactory : public QObject, public StandardToolFactory<QObject, CodecBrowser>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    virtual inline QString name() const { return tr( "Text Codecs" ); }
};

}

#endif // ENDOSCOPE_CODECBROWSER_H
