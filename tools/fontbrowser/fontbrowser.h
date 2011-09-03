#ifndef ENDOSCOPE_FONTBROWSER_H
#define ENDOSCOPE_FONTBROWSER_H

#include <qwidget.h>
#include <QFontDatabase>
#include <toolfactory.h>

class QItemSelection;

namespace Endoscope {

class FontModel;
namespace Ui { class FontBrowser; }

class FontBrowser : public QWidget
{
  Q_OBJECT
  public:
    explicit FontBrowser(ProbeInterface* probe, QWidget* parent = 0);

  private slots:
    void updateFonts(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QScopedPointer<Ui::FontBrowser> ui;
    FontModel *m_selectedFontModel;
};

class FontBrowserFactory : public QObject, public StandardToolFactory<QObject, FontBrowser>
{
  Q_OBJECT
  Q_INTERFACES(Endoscope::ToolFactory)
  public:
    virtual inline QString name() const { return tr( "Fonts" ); }
};

}

#endif // ENDOSCOPE_FONTBROWSER_H
