
#include "variantinspector.h"

#include <QWidget>

VariantInspector::VariantInspector(QObject *parent)
  : QObject(parent)
{
  m_widget.reset(new QWidget);
}

QSharedPointer<QWidget> VariantInspector::sharedWidget() const
{
  return m_widget;
}

QPointer<QWidget> VariantInspector::trackingWidget() const
{
  return m_widget.data();
}

QVector<int> VariantInspector::widgetVector() const
{
  QVector<int> vec;
  vec << 5;
  vec << 6;
  vec << 7;
  vec << 8;
  return vec;
}
