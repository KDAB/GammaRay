#ifndef GAMMARAY_ABOUTDIALOG_H
#define GAMMARAY_ABOUTDIALOG_H

#include <QDialog>

namespace GammaRay {

namespace Ui
{
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = 0);
    ~AboutDialog();

    void setTitle(const QString &title);
    void setText(const QString &text);
    void setLogo(const QString &iconFileName);

private:
    QScopedPointer<Ui::AboutDialog> ui;
};
}

#endif // GAMMARAY_ABOUTDIALOG_H
