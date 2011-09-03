#ifndef ENDOSCOPE_FONTBROWSER_H
#define ENDOSCOPE_FONTBROWSER_H

#include <qwidget.h>
#include <QFontDatabase>
#include <toolinterface.h>

class QItemSelection;

namespace Endoscope {

class FontModel;
class ProbeInterface;
namespace Ui { class FontBrowser; }

class FontBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit FontBrowser(ProbeInterface* probe, QWidget* parent = 0);
    virtual ~FontBrowser();

  private slots:
    void updateFonts(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    Ui::FontBrowser* ui;
    FontModel *m_selectedFontModel;
};

class FontBrowserInterface : public QObject, public StandardToolInterface<QObject, FontBrowser>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolInterface)
  public:
    virtual inline QString name() const { return tr( "Fonts" ); }
};

}

#endif // ENDOSCOPE_FONTBROWSER_H
