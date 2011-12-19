#include "propertydoublepaireditor.h"

#include "ui_propertydoublepaireditor.h"

using namespace GammaRay;

PropertyDoublePairEditor::PropertyDoublePairEditor(QWidget* parent): QWidget(parent), ui(new Ui::PropertyDoublePairEditor)
{
  ui->setupUi(this);
}

PropertyPointFEditor::PropertyPointFEditor(QWidget* parent): PropertyDoublePairEditor(parent)
{
}

QPointF PropertyPointFEditor::pointF() const
{
  return QPointF(ui->xBox->value(), ui->yBox->value());
}

void PropertyPointFEditor::setPointF(const QPointF& point)
{
  ui->xBox->setValue(point.x());
  ui->yBox->setValue(point.y());
}

PropertySizeFEditor::PropertySizeFEditor(QWidget* parent)
{
}

QSizeF PropertySizeFEditor::sizeF() const
{
  return QSizeF(ui->xBox->value(), ui->yBox->value());
}

void PropertySizeFEditor::setSizeF(const QSizeF& size)
{
  ui->xBox->setValue(size.width());
  ui->yBox->setValue(size.height());
}

#include "propertydoublepaireditor.moc"
