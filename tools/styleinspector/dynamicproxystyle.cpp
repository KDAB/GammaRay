#include "dynamicproxystyle.h"

#include <QApplication>

using namespace GammaRay;

QWeakPointer<DynamicProxyStyle> DynamicProxyStyle::s_instance;

DynamicProxyStyle::DynamicProxyStyle(QStyle* baseStyle): QProxyStyle(baseStyle)
{
  s_instance = QWeakPointer<DynamicProxyStyle>(this);
}

DynamicProxyStyle* DynamicProxyStyle::instance()
{
  if (!s_instance)
    insertProxyStyle();
  return s_instance.data();
}

bool DynamicProxyStyle::exists()
{
  return s_instance;
}

void DynamicProxyStyle::insertProxyStyle()
{
  // TODO: if the current style is a CSS proxy, add us underneath to avoid Qt adding yet another CSS proxy on top
  qApp->setStyle(new DynamicProxyStyle(qApp->style()));
}

void DynamicProxyStyle::setPixelMetric(QStyle::PixelMetric metric, int value)
{
  m_pixelMetrics.insert(metric, value);
}

int DynamicProxyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
  QHash<QStyle::PixelMetric, int>::const_iterator it = m_pixelMetrics.find(metric);
  if (it != m_pixelMetrics.end())
    return it.value();
  return QProxyStyle::pixelMetric(metric, option, widget);
}

#include "dynamicproxystyle.moc"
