#include "aboutdialog.h"
#include "ui_aboutdialog.h"

using namespace GammaRay;

AboutDialog::AboutDialog(QWidget* parent): QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::setTitle(const QString& title)
{
  ui->titleLabel->setText(title);
}

void AboutDialog::setText(const QString& text)
{
  ui->textLabel->setText(text);
}

void AboutDialog::setLogo(const QString& iconFileName)
{
  ui->logoLabel->setPixmap(iconFileName);
}

#include "aboutdialog.moc"