#include "propertyrecteditor.h"
#include "ui_propertyrecteditor.h"

using namespace GammaRay;

PropertyRectEditorDialog::PropertyRectEditorDialog(const QRectF &rect, QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::PropertyRectEditorDialog)
{
    ui->setupUi(this);

    ui->pointFWidget->setPointF(rect.topLeft());
    ui->sizeFWidget->setSizeF(rect.size());
    ui->stackedWidget->setCurrentWidget(ui->floatPage);
}

PropertyRectEditorDialog::PropertyRectEditorDialog(const QRect &rect, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyRectEditorDialog)
{
    ui->setupUi(this);

    ui->pointWidget->setPoint(rect.topLeft());
    ui->sizeWidget->setSizeValue(rect.size());
    ui->stackedWidget->setCurrentWidget(ui->intPage);
}

PropertyRectEditorDialog::~PropertyRectEditorDialog()
{
    delete ui;
}

QRectF PropertyRectEditorDialog::rectF() const
{
    if (ui->stackedWidget->currentWidget() == ui->intPage)
        return QRectF(ui->pointWidget->point(), ui->sizeWidget->sizeValue());
    else
        return QRectF(ui->pointFWidget->pointF(), ui->sizeFWidget->sizeF());
}

PropertyRectEditor::PropertyRectEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{

}

void PropertyRectEditor::edit()
{
    PropertyRectEditorDialog dlg(value().value<QRect>(), this);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.rectF().toRect());
}


PropertyRectFEditor::PropertyRectFEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyRectFEditor::edit()
{
    PropertyRectEditorDialog dlg(value().value<QRectF>(), this);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.rectF());
}

