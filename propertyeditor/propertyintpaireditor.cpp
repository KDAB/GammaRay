#include "propertyintpaireditor.h"

#include "ui_propertyintpaireditor.h"
#include <QDebug>

#include <limits>

using namespace GammaRay;

PropertyIntPairEditor::PropertyIntPairEditor(QWidget* parent): QWidget(parent), ui(new Ui::PropertyIntPairEditor)
{
  ui->setupUi(this);
  ui->xBox->setMinimum(std::numeric_limits<int>::min());
  ui->xBox->setMaximum(std::numeric_limits<int>::max());
  ui->yBox->setMinimum(std::numeric_limits<int>::min());
  ui->yBox->setMaximum(std::numeric_limits<int>::max());
}

PropertyPointEditor::PropertyPointEditor(QWidget* parent): PropertyIntPairEditor(parent)
{
}

QPoint PropertyPointEditor::point() const
{
  return QPoint(ui->xBox->value(), ui->yBox->value());
}

void PropertyPointEditor::setPoint(const QPoint& point)
{
  ui->xBox->setValue(point.x());
  ui->yBox->setValue(point.y());
}

PropertySizeEditor::PropertySizeEditor(QWidget* parent): PropertyIntPairEditor(parent)
{
}

QSize PropertySizeEditor::sizeValue() const
{
  return QSize(ui->xBox->value(), ui->yBox->value());
}

void PropertySizeEditor::setSizeValue(const QSize& size)
{
  ui->xBox->setValue(size.width());
  ui->yBox->setValue(size.height());
}

#include "propertyintpaireditor.moc"
